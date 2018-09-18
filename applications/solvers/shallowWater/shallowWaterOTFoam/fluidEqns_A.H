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

// --- PISO loop
while (piso.correct())
{
    UEqn.solve();
    U -= fvc::reconstruct
    (
        ocCoeff/(1+ocCoeff)*runTime.deltaT()
	*g*hf.oldTime()*fvc::snGrad(h.oldTime())*mesh.magSf()
    )/h;

    surfaceScalarField CoriolisFlux
    (
        runTime.deltaT()*CoriRecon*fvc::flux(h*(twoOmega ^ U))
    );
    surfaceScalarField phiHbyA
    (
        "phiHbyA",
        fvc::flux(h*U)
      - CoriolisFlux
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
          - fvm::laplacian(runTime.deltaT()/(1+ocCoeff)*hf*g, h, "laplacian(h)")
        );

        hEqn.solve(mesh.solver(h.select(piso.finalInnerIter())));

        if (piso.finalNonOrthogonalIter())
        {
            phi = phiHbyA + hEqn.flux();
        }
    }

    U -= fvc::reconstruct
    (
        runTime.deltaT()/(1+ocCoeff)*g*hf*fvc::snGrad(h)*mesh.magSf()
    )/h;
    U -= (U & meshNormal)*meshNormal;
    U.correctBoundaryConditions();

    {
        hUf = fvc::interpolate(h*U);
        surfaceVectorField n(mesh.Sf()/mesh.magSf());
        hUf += n*(fvc::absolute(phi,h,U)/mesh.magSf() - (n & hUf));
    }
    Uf = hUf/hf;
    hf = fvc::interpolate(h);
}

