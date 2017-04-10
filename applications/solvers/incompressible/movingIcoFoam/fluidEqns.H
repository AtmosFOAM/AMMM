// Calculate absolute flux from the mapped surface velocity
phi = rMesh.Sf() & Uf;

// Make the flux relative to the mesh motion
fvc::makeRelative(phi, U);

// Momentum predictor
fvVectorMatrix UEqn
(
    fvm::ddt(U)
  + fvm::div(phi, U)
);

if (piso.momentumPredictor())
{
    solve(UEqn == -fvc::grad(p));
}

// --- PISO loop
while (piso.correct())
{
    volScalarField rAU(1.0/UEqn.A());
    volVectorField HbyA(constrainHbyA(rAU*UEqn.H(), U, p));
    surfaceScalarField phiHbyA
    (
        "phiHbyA",
        fvc::flux(HbyA)
      + fvc::interpolate(rAU)*fvc::ddtCorr(U, Uf)
    );

    // Non-orthogonal pressure corrector loop
    while (piso.correctNonOrthogonal())
    {
        // Pressure corrector
        fvScalarMatrix pEqn
        (
            fvm::laplacian(rAU, p) == fvc::div(phiHbyA)
        );

        pEqn.setReference(pRefCell, pRefValue);

        pEqn.solve(rMesh.solver(p.select(piso.finalInnerIter())));

        if (piso.finalNonOrthogonalIter())
        {
            phi = phiHbyA - pEqn.flux();
        }
    }

    U = HbyA - rAU*fvc::grad(p);
    U.correctBoundaryConditions();

    {
        Uf = fvc::interpolate(U);
        surfaceVectorField n(rMesh.Sf()/rMesh.magSf());
        Uf += n*(phi/rMesh.magSf() - (n & Uf));
    }

    // Make the fluxes relative to the mesh motion
    fvc::makeRelative(phi, U);
}
