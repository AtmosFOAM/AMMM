#include "CourantNo.H"

//* Rewrite equations using OpenFOAM functions *//

if (colinParameter)
{
/*    // Reset A
    A     == dimensionedScalar("", dimless, scalar(1));
    A.oldTime() == dimensionedScalar("", dimless, scalar(1));
    AT    == T;
    AT.oldTime() == T.oldTime();
    AuniT == uniT;
    AuniT.oldTime() == uniT.oldTime();
*/
    // Make the flux relative
    fvc::makeRelative(phi, U);
    fvc::makeRelative(zeroPhi, U);

    // Get flow over the ground for debugging
    phi.boundaryFieldRef()[5] *= 0;
    forAll(phi.boundaryFieldRef()[5], facei)
    {
        phi.boundaryFieldRef()[5][facei] = 0;
    }
    flowOverGround.ref().field() = phi.boundaryField()[5];

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
            fvm::ddt(AT)
          + fvc::div(phi, AT, "div(phi,T)")
        );
        ATEqn.solve();

        fvScalarMatrix AuniTEqn
        (
            fvm::ddt(AuniT)
          + fvc::div(phi, AuniT, "div(phi,T)")
        );
        AuniTEqn.solve();
    }

    // Update T and uniT
    T     == AT/A;
    uniT  == AuniT/A;

    // Recover the absolute flux
    fvc::makeAbsolute(phi, U);
    fvc::makeAbsolute(zeroPhi, U);
}
else
{
    // Make the flux relative
    fvc::makeRelative(phi, U);

    // Get flow over the ground for debugging
    flowOverGround.ref().field() = phi.boundaryField()[5];

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

    // Recover the absolute flux
    fvc::makeAbsolute(phi, U);
}
