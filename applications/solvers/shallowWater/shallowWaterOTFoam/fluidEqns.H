// Calculate absolute flux from the mapped surface velocity
phi = hUf & mesh.Sf();

// Make the flux relative to the mesh motion
fvc::makeRelative(phi, h0, U);

// Momentum predictor
fvVectorMatrix UEqn
(
    fvm::ddt(h0,U)
  + fvc::div(phi, U)
//  + h0*(twoOmega ^ U)
);

if (piso.momentumPredictor())
{
    solve(UEqn == -h0*g*fvc::grad(h));
}

// --- PISO loop
while (piso.correct())
{
    volScalarField rAU(1.0/UEqn.A());
    surfaceScalarField hrAUf("hrAUf", fvc::interpolate(h0*rAU));
    volVectorField HbyA(constrainHbyA(rAU*UEqn.H(), U, h));
    surfaceScalarField CoriolisFlux(hf*fvc::flux(h0*rAU*(twoOmega ^ U)));
    surfaceScalarField phiHbyA
    (
        "phiHbyA",
        hf*fvc::flux(HbyA)
      - CoriolisFlux
      + hrAUf*fvc::ddtCorr(h0, U, hUf)
    );
    fvc::makeRelative(phiHbyA, h0, U);

    // Non-orthogonal pressure corrector loop
    while (piso.correctNonOrthogonal())
    {
        // Pressure corrector
        fvScalarMatrix hEqn
        (
            /*fvm::ddt(h)
          + */fvc::div(phiHbyA)
          - fvm::laplacian(hrAUf*hf*g, h, "laplacian(h)")
        );
        hEqn.setReference(0,0);

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

/*    // Update the velocity, blending A and C-grids (C is reconstruct from phi)
    U = (1-ACblend)*
        (
            HbyA - fvc::reconstruct
            (
                hrAUf*hf*g*fvc::snGrad(h)*mesh.magSf()
//              + CoriolisFlux
            )/h0
        )
      + ACblend*fvc::reconstruct(phi/hf);
    U.correctBoundaryConditions();*/
    U = HbyA - fvc::reconstruct
    (
        hrAUf*hf*g*fvc::snGrad(h)*mesh.magSf()
      + CoriolisFlux
    )/h0;
    U -= (U & meshNormal)*meshNormal;
    U.correctBoundaryConditions();
    //U = hU/h0;

    {
        Uf = fvc::interpolate(U);
        surfaceVectorField n(mesh.Sf()/mesh.magSf());
        Uf += n*(phi/(hf*mesh.magSf()) - (n & Uf));
    }
    hf = fvc::interpolate(h0);
    hUf = hf*Uf;

    // Make the fluxes relative to the mesh motion
    fvc::makeRelative(phi, h0, U);
}
divPhi = fvc::div(phi);

