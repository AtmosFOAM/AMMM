// Calculate absolute flux from the mapped surface velocity
phi = hf*(fvc::interpolate(U) & mesh.Sf());

phi = phi - runTime.deltaT()*hf*
(
    fvc::flux
    (
        fvc::div(phi,U)/h
      + (twoOmega ^ U)
    )
  + g*fvc::snGrad(h)*mesh.magSf()
);
phi.write();
divPhi = fvc::div(phi);
divPhi.write();
FatalErrorIn("fluidEqns.H") << exit(FatalError);


// Make the flux relative to the mesh motion
fvc::makeRelative(phi, h, U);

// Momentum predictor
fvVectorMatrix UEqn
(
    fvm::ddt(h,U)
  + fvc::div(phi, U)
//  + h*(twoOmega ^ U)
);

//if (piso.momentumPredictor())
//{
//    solve(UEqn == -h*g*fvc::grad(h));
//}

//Info << "(UEqn.H + fvc::div(phi,U)) & U/U^2/h = "
//     << ((UEqn.H() + fvc::div(phi,U))&U)/magSqr(U)/h.oldTime() << endl;
//Info << "UEqn.A/h = " << UEqn.A()/h << endl;
//FatalErrorIn("fluidEqns.H") << exit(FatalError);

// --- PISO loop
while (piso.correct())
{
    hf = fvc::interpolate(h);
    volScalarField rAU(1.0/UEqn.A());
    surfaceScalarField hrAUf("hrAUf", fvc::interpolate(h*rAU));
    volVectorField HbyA(constrainHbyA(rAU*UEqn.H(), U, h));
    surfaceScalarField CoriolisFlux(hf*fvc::flux(h*rAU*(twoOmega ^ U)));
    surfaceScalarField phiHbyA
    (
        "phiHbyA",
        hf*fvc::flux(HbyA)
      - CoriolisFlux
      + hrAUf*fvc::ddtCorr(h, U, hUf)
    );

    fvc::makeRelative(phiHbyA, h, U);

    // Non-orthogonal pressure corrector loop
    while (piso.correctNonOrthogonal())
    {
        // Pressure corrector
        fvScalarMatrix hEqn
        (
            fvm::ddt(h)
          + fvc::div(phiHbyA)
          - fvm::laplacian(hrAUf*hf*g, h, "laplacian(h)")
        );

        hEqn.solve(mesh.solver(h.select(piso.finalInnerIter())));

        if (piso.finalNonOrthogonalIter())
        {
            //phi = phiHbyA + hEqn.flux();            
            phi = phiHbyA - hrAUf*hf*g*fvc::snGrad(h)*mesh.magSf();
            //U = HbyA + fvc::reconstruct(hEqn.flux())/h;
        }
    }

    //U = HbyA - rAU*g*h*fvc::grad(h);
    //U = HbyA - fvc::reconstruct(hrAUf*g*fvc::snGrad(h)*mesh.magSf());
    U = HbyA - fvc::reconstruct
    (
        hrAUf*hf*g*fvc::snGrad(h)*mesh.magSf()
      - CoriolisFlux
    )/h;
    //U = fvc::reconstruct(phi/hf);
    //U = fvc::reconstruct(phi)/h;
    U.correctBoundaryConditions();

    {
        hUf = fvc::interpolate(h*U);
        surfaceVectorField n(mesh.Sf()/mesh.magSf());
        hUf += n*(phi/(mesh.magSf()) - (n & hUf));
    }

    // Make the fluxes relative to the mesh motion
    fvc::makeRelative(phi, h, U);
}
