#include "CourantNo.H"

// Make the flux relative
fvc::makeRelative(phi, U);

// Get flow over the ground for debugging
flowOverGround.ref().field() = phi.boundaryField()[5];

if (colinParameter)
{
    // Reset A
    A     == dimensionedScalar("", dimless, scalar(1));
    A.oldTime() == dimensionedScalar("", dimless, scalar(1));

    // Solve equations iterativelly
    for(int icorr = 0; icorr < nRKstages; icorr++)
    {
        fvScalarMatrix AEqn
        (
            fvm::ddt(A)
          + fvc::div(phi, A, "div(phi,T)")
        );
        AEqn.solve();

        fvScalarMatrix ATEqn
        (
            fvm::ddt(A,T)
          + fvc::div(phi, A*T, "div(phi,T)")
        );
        ATEqn.solve();

        fvScalarMatrix AuniTEqn
        (
            fvm::ddt(A, uniT)
          + fvc::div(phi, A*uniT, "div(phi,T)")
        );
        AuniTEqn.solve();
    }
}
else
{
    // Solve equations iterativelly
    for(int icorr = 0; icorr < nRKstages; icorr++)
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
}

// Recover the absolute flux
fvc::makeAbsolute(phi, U);

