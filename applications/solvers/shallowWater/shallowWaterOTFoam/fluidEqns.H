// Calculate the orography on the moved mesh
if (!fixedMesh)
{
    fvScalarMatrix h0Eqn
    (
        fvm::ddt(h0) - fvc::div(mesh.phi(), h0)
    );
    h0Eqn.solve();
}

if (includeSponge)
{
    fvTensorMatrix muSpongeEqn
    (
        fvm::ddt(muSponge) - fvc::div(mesh.phi(), muSponge)
    );
    muSpongeEqn.solve();
}

// Calculate absolute flux from the mapped surface velocity
phi = hUf & mesh.Sf();

// Make the flux relative to the mesh motion
fvc::makeRelative(phi, h, U);

// Momentum predictor
fvVectorMatrix UEqn
(
    fvm::ddt(h,U)
  + fvm::div(phi, U)
  + (1-CoriRecon)*h*(twoOmega ^ U)
  + fvc::reconstruct(hf*g*fvc::snGrad(h0)*mesh.magSf())
);
if (includeSponge)
{
    UEqn += h/runTime.deltaT()*(muSponge & U);
}
if (includeDiffusion)
{
    UEqn -= fvm::laplacian(diffusionCoeff*h, U);
}

if (piso.momentumPredictor())
{
    solve(UEqn); // == -CoriRecon*h*(twoOmega ^ U) - h*g*fvc::grad(h));
}
/*
fvVectorMatrix U2Eqn
(
    fvm::ddt(h,U)
  + fvc::div(phi, U)
  + fvc::reconstruct(hf*g*fvc::snGrad(h0)*mesh.magSf())
  - fvm::laplacian(diffusionCoeff*h, U)
);
*/
// --- PISO loop
while (piso.correct())
{
    volScalarField rAU(1.0/UEqn.A());
    surfaceScalarField hrAUf("hrAUf", hf*fvc::interpolate(rAU));
//    surfaceScalarField hrAUf("hrAUf", fvc::interpolate(h*rAU));
    volVectorField HbyA("HbyA",constrainHbyA(rAU*UEqn.H(), U, h));

    surfaceScalarField CoriolisFlux
    (
        CoriRecon*hf*fvc::flux(h*rAU*(twoOmega ^ U))
    );
    surfaceScalarField phiHbyA
    (
        "phiHbyA",
//        hf*fvc::flux(HbyA)
        fvc::flux(h*HbyA)
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
//          - fvm::laplacian(runTime.deltaT()*hf*g, h, "laplacian(h)")
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
        hUf = hf*fvc::interpolate(U);
//        hUf = fvc::interpolate(h*U);
        surfaceVectorField n(mesh.Sf()/mesh.magSf());
        hUf += n*(fvc::absolute(phi,h,U)/mesh.magSf() - (n & hUf));
    }
    Uf = hUf/hf;
    hf = fvc::interpolate(h);
}

