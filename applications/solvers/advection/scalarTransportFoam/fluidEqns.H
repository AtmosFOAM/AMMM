// Make the flux relative
#include "CourantNo.H"

A == dimensionedScalar("", dimless, scalar(1));fvc::makeRelative(phi, U);

fvc::makeRelative(phi.oldTime(), U.oldTime());

// First parts of Crank-Nicolson time-stepping
Aold      == volRatio*A
           - volRatio*dt*(1-offCentre)*fvc::div(phi.oldTime(), A, "div(phi,A)");
uniTold   == volRatio*uniT
           - volRatio*dt*(1-offCentre)*fvc::div(phi.oldTime(), uniT, "div(phi,A)");

ATold     == volRatio*AT
           - volRatio*dt*(1-offCentre)*fvc::div(phi, AT, "div(phi,A)");

// Fixed number of iterations per time-step
for (int corr = 0; corr < nRKstages; corr++)
{
    A     == Aold      - dt*offCentre*fvc::div(phi, A, "div(phi,A)");
    uniT  == uniTold   - dt*offCentre*fvc::div(phi, uniT, "div(phi,A)");

    AT    == ATold - dt*offCentre*fvc::div(phi, AT, "div(phi,A)");
    T == AT/A;
}

fvc::makeAbsolute(phi, U);
fvc::makeAbsolute(phi.oldTime(), U.oldTime());

