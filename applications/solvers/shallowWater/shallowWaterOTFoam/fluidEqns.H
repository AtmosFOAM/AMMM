// Calculate absolute flux from the mapped surface velocity
phi = hUf & mesh.Sf();

// Make the flux relative to the mesh motion
fvc::makeRelative(phi, h, U);

// Momentum predictor
fvVectorMatrix UEqn
(
    fvm::ddt(h,U)
  + fvc::div(phi, U)
  + (1-CoriRecon)*h*(twoOmega ^ U)
);

if (piso.momentumPredictor())
{
    solve(UEqn == -CoriRecon*h*(twoOmega ^ U) - h*g*fvc::grad(h));
}

// --- PISO loop
while (piso.correct())
{
    volScalarField rAU(1.0/UEqn.A());
    surfaceScalarField hrAUf("hrAUf", fvc::interpolate(h*rAU));
    volVectorField HbyA(constrainHbyA(rAU*UEqn.H(), U, h));
    surfaceScalarField CoriolisFlux
    (
        CoriRecon*hf*fvc::flux(h*rAU*(twoOmega ^ U))
    );
    surfaceScalarField phiHbyA
    (
        "phiHbyA",
        hf*fvc::flux(HbyA)
      - CoriolisFlux
      + ACblend*hrAUf*fvc::ddtCorr(h, U, hUf)
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

    U = HbyA - fvc::reconstruct
    (
        hrAUf*hf*g*fvc::snGrad(h)*mesh.magSf()
      + CoriolisFlux
    )/h;
    U -= (U & meshNormal)*meshNormal;
    U.correctBoundaryConditions();

    {
        Uf = fvc::interpolate(U);
        surfaceVectorField n(mesh.Sf()/mesh.magSf());
        Uf += n*(phi/(hf*mesh.magSf()) - (n & Uf));
    }
    hf = fvc::interpolate(h);
    hUf = hf*Uf;

    // Make the fluxes relative to the mesh motion
    fvc::makeRelative(phi, h, U);
}
divPhi = fvc::div(phi);

