if (colinParameter)
{
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

    // Solve T and uniT equations iterativelly
    for(int icorr = 0; icorr < nIteration; icorr++)
    {
        fvScalarMatrix AuniTEqn
        (
            fvm::ddt(A, uniT)
          + (1-offCentre)*fvc::div(phi, uniT.oldTime(), "div(phi,T)")
          + offCentre*fvc::div(phi, uniT, "div(phi,T)")
        );
        AuniTEqn.solve();

        fvScalarMatrix ATEqn
        (
            fvm::ddt(A, T)
          + (1-offCentre)*fvc::div(phi, T.oldTime(), "div(phi,T)")
          + offCentre*fvc::div(phi, T, "div(phi,T)")
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
        fvScalarMatrix uniTEqn
        (
            fvm::ddt(uniT)
          + (1-offCentre)*fvc::div(phi, uniT.oldTime(), "div(phi,T)")
          + offCentre*fvc::div(phi, uniT, "div(phi,T)")
        );
        uniTEqn.solve();

        fvScalarMatrix TEqn
        (
            fvm::ddt(T)
          + (1-offCentre)*fvc::div(phi, T.oldTime(), "div(phi,T)")
          + offCentre*fvc::div(phi, T, "div(phi,T)")
        );
        TEqn.solve();
    }

    // Make the fluxes absolute
    fvc::makeAbsolute(phi, U);
}
