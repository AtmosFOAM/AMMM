// Make the flux relative
#include "CourantNo.H"

fvc::makeRelative(phi, U);

// First parts of Crank-Nicolson time-stepping
Aold      == volRatio*A
           - volRatio*dt*(1-offCentre)*fvc::div(phi, A, "div(phi,T)");
Aold*Told == volRatio*A*T
           - volRatio*dt*(1-offCentre)*fvc::div(phi, A*T, "div(phi,T)");
uniTold   == volRatio*uniT
           - volRatio*dt*(1-offCentre)*fvc::div(phi, uniT, "div(phi,T)");

// Fixed number of iterations per time-step
for (int corr = 0; corr < nRKstages; corr++)
{
    A    == Aold      - dt*offCentre*fvc::div(phi, A, "div(phi,T)");
    A*T  == Aold*Told - dt*offCentre*fvc::div(phi, A*T);
    uniT == uniTold   - dt*offCentre*fvc::div(phi, uniT, "div(phi,T)");
}

fvc::makeAbsolute(phi, U);

