if (colinParameter)
{
    surfaceScalarField meshFlux("meshFlux", mesh.phi());

    // Solve A equation implicitly
    fvScalarMatrix AmeshFlux
    (
        fvm::ddt(A) + fvm::div(-meshFlux, A)
    );
    AmeshFlux.solve();

    // Modify the mesh fluxes
    meshFlux *= fvc::interpolate(A);

    // Solve A again based on the modified mesh fluxes
    fvScalarMatrix AEqn
    (
        fvm::ddt(A) + fvc::div(-meshFlux)
    );
    AEqn.solve();

    // Make the fluxes relative
    phi -= meshFlux;

    // Solve T and uniT equations iterativelly
    for(int icorr = 0; icorr < nIteration; icorr++)
    {
        fvScalarMatrix AuniTEqn
        (
            fvm::ddt(A, uniT)
          + fvc::div(phi, uniT, "div(phi,T)")
        );
        AuniTEqn.solve();

        fvScalarMatrix ATEqn
        (
            fvm::ddt(A, T)
          + fvc::div(phi, T, "div(phi,T)")
        );
        ATEqn.solve();
    }

    // Make the fluxes absolute
    phi += meshFlux;
}
else
{
    // Make the fluxes relative
    fvc::makeRelative(phi, U);

    // Solve T and uniT equations iterativelly
    for(int icorr = 0; icorr < nIteration; icorr++)
    {
        fvScalarMatrix TEqn
        (
            fvm::ddt(T)
          + fvc::div(phi, T)
        );
        TEqn.solve();

        fvScalarMatrix uniTEqn
        (
            fvm::ddt(uniT)
          + fvc::div(phi, uniT, "div(phi,T)")
        );
        uniTEqn.solve();
    }

    // Make the fluxes absolute
    fvc::makeAbsolute(phi, U);
}
