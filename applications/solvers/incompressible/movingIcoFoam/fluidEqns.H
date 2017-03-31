// Calculate absolute flux from the mapped surface velocity
phi = rMesh.Sf() & Uf;

//CorrectPhi
//(
//    U, phi, p, dimensionedScalar("rAUf", dimTime, 1), geometricZeroField(),
//    piso
//);

// Make the flux relative to the mesh motion
fvc::makeRelative(phi, U);

//MRF.correctBoundaryVelocity(U);

// Momentum predictor
tmp<fvVectorMatrix> tUEqn
(
    fvm::ddt(U)
  + fvm::div(phi, U)
);
fvVectorMatrix& UEqn = tUEqn.ref();

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

//    MRF.makeRelative(phiHbyA);

//    if (p.needReference())
//    {
//        fvc::makeRelative(phiHbyA, U);
//        adjustPhi(phiHbyA, U, p);
//        fvc::makeAbsolute(phiHbyA, U);
//    }

    tmp<volScalarField> rAtU(rAU);

//    if (piso.consistent())
//    {
//        rAtU = 1.0/max(1.0/rAU - UEqn.H1(), 0.1/rAU);
//        phiHbyA +=
//            fvc::interpolate(rAtU() - rAU)*fvc::snGrad(p)*mesh.magSf();
//        HbyA -= (rAU - rAtU())*fvc::grad(p);
//    }

    if (piso.nCorrPISO() <= 1)
    {
        tUEqn.clear();
    }

//    // Update the pressure BCs to ensure flux consistency
//    constrainPressure(p, U, phiHbyA, rAtU(), MRF);

    // Non-orthogonal pressure corrector loop
    while (piso.correctNonOrthogonal())
    {
        // Pressure corrector

        fvScalarMatrix pEqn
        (
            fvm::laplacian(rAtU(), p) == fvc::div(phiHbyA)
        );

        pEqn.setReference(pRefCell, pRefValue);

        pEqn.solve(rMesh.solver(p.select(piso.finalInnerIter())));

        if (piso.finalNonOrthogonalIter())
        {
            phi = phiHbyA - pEqn.flux();
        }
    }

    U = HbyA - rAtU*fvc::grad(p);
    U.correctBoundaryConditions();

    {
        Uf = fvc::interpolate(U);
        surfaceVectorField n(rMesh.Sf()/rMesh.magSf());
        Uf += n*(phi/rMesh.magSf() - (n & Uf));
    }

    // Make the fluxes relative to the mesh motion
    fvc::makeRelative(phi, U);
}
