// Calculate absolute flux from the mapped surface velocity
phi = Uf & mesh.Sf();

// Make the flux relative to the mesh motion
fvc::makeRelative(phi, U);

const scalar CoriRecon = 1;

// Momentum predictor
fvVectorMatrix UEqn
(
    fvm::ddt(U)
  + fvm::div(phi, U)
  + (1-CoriRecon)*(twoOmega ^ U)
);

if (piso.momentumPredictor())
{
    solve(UEqn == -CoriRecon*(twoOmega ^ U) -fvc::grad(p));
}

// --- PISO loop
while (piso.correct())
{
    volScalarField rAU(1.0/UEqn.A());
    volVectorField HbyA(constrainHbyA(rAU*UEqn.H(), U, p));
    surfaceScalarField CoriolisFlux(CoriRecon*fvc::flux(rAU*(twoOmega ^ U)));
    surfaceScalarField phiHbyA
    (
        "phiHbyA",
        fvc::flux(HbyA)
      - CoriolisFlux
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

        pEqn.solve(mesh.solver(p.select(piso.finalInnerIter())));

        if (piso.finalNonOrthogonalIter())
        {
            phi = phiHbyA - pEqn.flux();
        }
    }

//    U = HbyA - rAU*(CoriRecon*(twoOmega ^ U) + fvc::grad(p));
    U = HbyA - fvc::reconstruct
    (
        fvc::interpolate(rAU)*fvc::snGrad(p)*mesh.magSf()
      + CoriolisFlux
    );
    U -= (U & meshNormal)*meshNormal;
    U.correctBoundaryConditions();

    {
        Uf = fvc::interpolate(U);
        surfaceVectorField n(mesh.Sf()/mesh.magSf());
        Uf += n*(phi/mesh.magSf() - (n & Uf));
    }

    // Make the fluxes relative to the mesh motion
    fvc::makeRelative(phi, U);
}

