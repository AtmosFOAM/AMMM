// Calculate absolute flux from the mapped surface velocity
phi = hUf & mesh.Sf();

// Make the flux relative to the mesh motion
fvc::makeRelative(phi, h, U);

// Momentum predictor
fvVectorMatrix UEqn
(
    fvm::ddt(h,U)
  + fvc::div(phi, U)
  + h*(twoOmega ^ U)
);

// fvc::makeRelative(phi, U);

// fvScalarMatrix TEqn
// (
//     fvm::ddt(T)
//   + fvc::div(phi, T)
// );
// TEqn = 0;

// --- PISO loop
while (piso.correct())
{
    volScalarField rAU(1.0/UEqn.A());
    surfaceScalarField hrAUf("hrAUf", fvc::interpolate(h*rAU));
    volVectorField HbyA(constrainHbyA(rAU*UEqn.H(), U, h));
    surfaceScalarField phiHbyA
    (
        "phiHbyA",
        fvc::flux(h*HbyA)
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

