// Make the flux relative
#include "CourantNo.H"

fvc::makeRelative(phi, U);

// First parts of Crank-Nicolson time-stepping
Aold      == volRatio*A
           - volRatio*dt*(1-offCentre)*fvc::div(phi, A);
Told      == volRatio*T
           - volRatio*dt*(1-offCentre)*fvc::div(phi, T, "div(phi,A)");
uniTold   == volRatio*uniT
           - volRatio*dt*(1-offCentre)*fvc::div(phi, uniT, "div(phi,A)");

AT        == A*T;
ATold     == Aold*Told;
ATold     == volRatio*AT
           - volRatio*dt*(1-offCentre)*fvc::div(phi, AT, "div(phi,A)");

// Fixed number of iterations per time-step
for (int corr = 0; corr < nRKstages; corr++)
{
    A     == Aold      - dt*offCentre*fvc::div(phi, A, "div(phi,A)");
    T     == Told      - dt*offCentre*fvc::div(phi, T, "div(phi,A)");
    uniT  == uniTold   - dt*offCentre*fvc::div(phi, uniT, "div(phi,A)");

    AT    == A*T;
    ATold == Aold*Told;
    AT    == ATold     - dt*offCentre*fvc::div(phi, AT, "div(phi,A)");
}

fvc::makeAbsolute(phi, U);
