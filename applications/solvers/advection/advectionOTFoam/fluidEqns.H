// Make the flux relative
fvc::makeRelative(phi, U);

flowOverGround.ref().field() = phi.boundaryField()[5];

if (colinParameter)
{
    // Reset A
    if (resetA)
    {
        A.oldTime() == dimensionedScalar("", dimless, scalar(1));
    }

    fvScalarMatrix AEqn
    (
        fvm::ddt(A)
      - fvc::div(mesh.phi())
    );
    AEqn.solve();

    // Solve equations iterativelly
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
    AT == A*T;
}
else
{
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
    AT == A*T;
}

// Recover the absolute flux
fvc::makeAbsolute(phi, U);
