if (colinParameter)
{
    // Reset A to unity
    if (resetA)
    {
        A.oldTime() == dimensionedScalar("", dimless, scalar(1));
    }

    surfaceScalarField meshFlux("meshFlux", mesh.phi());

    // Formulation where the mesh flux is multiplied by A (upwinded)
    if (AmeshFlux)
    {
        // Solve A equation
        fvScalarMatrix AEqn
        (
            fvm::ddt(A) + fvm::div(-meshFlux, A)
        );
        AEqn.solve();

        // Modify the mesh fluxes and calculate final A
        meshFlux *= fvc::interpolate(A);
    }

    // Set A based on mesh fluxes (with or without A)
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

    // Make the fluxes absolute again
    phi += meshFlux;
}
else
{
    // Make the flux relative
    fvc::makeRelative(phi, U);

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
    // Recover the absolute flux
    fvc::makeAbsolute(phi, U);
}
