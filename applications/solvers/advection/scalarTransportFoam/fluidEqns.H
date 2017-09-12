// Make the flux relative
#include "CourantNo.H"

A == dimensionedScalar("", dimless, scalar(1));

fvc::makeRelative(phi, U);
fvc::makeRelative(zeroPhi, U0);

forAll(phi.boundaryFieldRef()[5], facei)
{
    zeroPhi.boundaryFieldRef()[4][facei] = 0;
    zeroPhi.boundaryFieldRef()[5][facei] = 0;
    phi.boundaryFieldRef()[4][facei] = 0;
    phi.boundaryFieldRef()[5][facei] = 0;
}

flowOverGround.ref().field() = phi.boundaryField()[5];

// First parts of Crank-Nicolson time-stepping

Aold      == volRatio*A
           - volRatio*dt*(1-offCentre)*fvc::div(zeroPhi, A, "div(phi,A)");

AT == T;
ATold     == volRatio*AT
           - volRatio*dt*(1-offCentre)*fvc::div(phi, AT, "div(phi,A)");

AuniT == uniT;
AuniTold  == volRatio*AuniT
           - volRatio*dt*(1-offCentre)*fvc::div(phi, AuniT, "div(phi,A)");

// Fixed number of iterations per time-step
for (int corr = 0; corr < nRKstages; corr++)
{
    A     == Aold      - dt*offCentre*fvc::div(zeroPhi, A, "div(phi,A)");

    AT    == ATold - dt*offCentre*fvc::div(phi, AT, "div(phi,A)");
    T     == AT/A;

    AuniT == AuniTold - dt*offCentre*fvc::div(phi, AuniT, "div(phi,A)");
    uniT  == AuniT/A;
}

fvc::makeAbsolute(phi, U);
fvc::makeAbsolute(zeroPhi, U0);
// fvc::makeAbsolute(phi.oldTime(), U.oldTime());
