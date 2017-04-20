// Calculate absolute flux from the mapped surface velocity
phi = hf*(fvc::interpolate(U) & mesh.Sf());

// Make the flux relative to the mesh motion
fvc::makeRelative(phi, h, U);

// Momentum predictor
fvVectorMatrix UEqn
(
    fvm::ddt(h,U)
  + fvc::div(phi, U)
//  + h*(twoOmega ^ U)
);

if (piso.momentumPredictor())
{
    solve(UEqn == -h*g*fvc::grad(h));
}

// --- PISO loop
while (piso.correct())
{
    volScalarField rAU(1.0/UEqn.A());
    surfaceScalarField hrAUf("hrAUf", fvc::interpolate(h*rAU));
    volVectorField HbyA(constrainHbyA(rAU*UEqn.H(), U, h));
    surfaceScalarField CoriolisFlux(hf*fvc::flux(h*rAU*(twoOmega ^ U)));
    surfaceScalarField phiHbyA
    (
        "phiHbyA",
        hf*fvc::flux(HbyA)
      - CoriolisFlux
//      + hrAUf*fvc::ddtCorr(h, U, hUf)
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
            phi = phiHbyA + hEqn.flux();
        }
    }

    // Blend between the A-grid and the C-grid
    del2h = fvc::laplacian(h);
    del4h = fvc::laplacian(del2h, "laplacian(h)");
    if (ACblendConst < -SMALL)
    {
        ACblend = calcACblend(del4h);
    }

    // Update the velocity, blending A and C-grids (C is reconstruct from phi)
    U = (1-ACblend)*
        (
            HbyA - fvc::reconstruct
            (
                hrAUf*hf*g*fvc::snGrad(h)*mesh.magSf()
              + CoriolisFlux
            )/h
        )
      + ACblend*fvc::reconstruct(phi/hf);
    U.correctBoundaryConditions();

    {
        hUf = fvc::interpolate(h*U);
        surfaceVectorField n(mesh.Sf()/mesh.magSf());
        hUf += n*(phi/(mesh.magSf()) - (n & hUf));
    }
    hf = fvc::interpolate(h);

    // Make the fluxes relative to the mesh motion
    fvc::makeRelative(phi, h, U);
}
divPhi = fvc::div(phi);

