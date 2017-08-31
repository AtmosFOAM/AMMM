// Make the flux relative
#include "CourantNo.H"

fvc::makeRelative(phi, U);

// First parts of Crank-Nicolson time-stepping
Told == volRatio*T
      - volRatio*dt*(1-offCentre)*fvc::div(phi, T, "div(phi,T)");
uniTold == volRatio*uniT
         - volRatio*dt*(1-offCentre)*fvc::div(phi, uniT, "div(phi,T)");

// Fixed number of iterations per time-step
for (int corr = 0; corr < nRKstages; corr++)
{
    T    == Told    - dt*offCentre*fvc::div(phi, T);
    uniT == uniTold - dt*offCentre*fvc::div(phi, uniT, "div(phi,T)");
}

fvc::makeAbsolute(phi, U);

