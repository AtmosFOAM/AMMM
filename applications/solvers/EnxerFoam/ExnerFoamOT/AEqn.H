// Solve the equation for A, the volume adjustment parameter
// and modify the mesh flux to include A

surfaceScalarField meshFlux("meshFlux", mesh.phi());

// Modify the mesh fluxes
meshFlux *= fvc::interpolate(A);

// Solve A based on the modified mesh fluxes
fvScalarMatrix AEqn
(
    fvm::ddt(A) - fvc::div(meshFlux)
);
AEqn.solve();

// Make the fluxes relative
phi -= meshFlux;


