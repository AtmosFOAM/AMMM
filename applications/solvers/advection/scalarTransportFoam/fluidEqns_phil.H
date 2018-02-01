#include "CourantNo.H"

if (colinParameter)
{
    // Reset A
    A     == dimensionedScalar("", dimless, scalar(1));
    AT    == T;
    AuniT == uniT;

    // Make the flux relative
    fvc::makeRelative(phi, U);
    fvc::makeRelative(zeroPhi, U0);

    // Get flow over the ground for debugging
    flowOverGround.ref().field() = phi.boundaryField()[5];

    // First parts of Crank-Nicolson time-stepping
    Aold     == volRatio*A
              - volRatio*dt*(1-offCentre)*fvc::div(phi, A,     "div(phi,T)");

    ATold    == volRatio*AT
              - volRatio*dt*(1-offCentre)*fvc::div(phi, AT,    "div(phi,T)");

    AuniTold == volRatio*AuniT
              - volRatio*dt*(1-offCentre)*fvc::div(phi, AuniT, "div(phi,T)");

    // Fixed number of iterations per time-step
    for (int corr = 0; corr < nRKstages; corr++)
    {
        A     == Aold     - dt*offCentre*fvc::div(phi,  A,     "div(phi,T)");
        AT    == ATold    - dt*offCentre*fvc::div(phi,  AT,    "div(phi,T)");
        AuniT == AuniTold - dt*offCentre*fvc::div(phi,  AuniT, "div(phi,T)");
    }

    // Update T and uniT
    T     == AT/A;
    uniT  == AuniT/A;

    // Recover the absolute flux
    fvc::makeAbsolute(phi, U);
    fvc::makeAbsolute(zeroPhi, U0);
}
else
{
    // Make the flux relative
    fvc::makeRelative(phi, U);

    // Get flow over the ground for debugging
    flowOverGround.ref().field() = phi.boundaryField()[5];

    // First parts of Crank-Nicolson time-stepping
    Told     == volRatio*T
              - volRatio*dt*(1-offCentre)*fvc::div(phi, T,    "div(phi,T)");

    uniTold  == volRatio*uniT
              - volRatio*dt*(1-offCentre)*fvc::div(phi, uniT, "div(phi,T)");

    // Fixed number of iterations per time-step
    for (int corr = 0; corr < nRKstages; corr++)
    {
        T    == Told    - dt*offCentre*fvc::div(phi, T,    "div(phi,T)");
        uniT == uniTold - dt*offCentre*fvc::div(phi, uniT, "div(phi,T)");
    }

    // Recover the absolute flux
    fvc::makeAbsolute(phi, U);
}
