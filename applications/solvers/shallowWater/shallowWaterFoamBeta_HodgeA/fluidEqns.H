fvc::makeRelative(U, h, hu);
#include "CourantNo.H"

// Old part of the momentum
volVectorField dhudt = -fvc::div(U,u) - (twoOmega ^ hu) - g*h*fvc::grad(h);
volVectorField huOld = volRatio*hu.oldTime() + (1-offCentre)*dt*dhudt;

// Explicit Momentum update
hu = huOld + offCentre*dt*dhudt;
hu.correctBoundaryConditions();
u == hu/h;

// Explicit momentum in d direction without Coriolis or pressure gradient
V = H.delta() & fvc::interpolate
(
    huOld
  - offCentre*dt*fvc::div(U,u)
);

// Solve pressure equation
for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
{
    // In this loop, U is NOT relative
    U = H.ddirToFlux
    (
        V - offCentre*dt*(H.delta() & fvc::interpolate(twoOmega ^ hu))
    )
  - H.ddirToFluxCorr(dt*g*hf*fvc::snGrad(h)*H.magd());

    fvScalarMatrix hEqn
    (
        fvm::ddt(h)
      - fvc::div(hf*fvc::meshPhi(u))
      + (1-offCentre)*divUold
      + offCentre*fvc::div(U)
      - fvm::laplacian(dt*sqr(offCentre)*g*hf, h, "h")
    );

    if(nonOrth == nNonOrthCorr)
    {
        hEqn.solve(h.name() + "Final");
    }
    else
    {
        hEqn.solve(h.name());
    }

    if (nonOrth == nNonOrthCorr)
    {
        U += hEqn.flux()/offCentre;
    }
}

// Updates for next time step
dhudt = -fvc::div(U,u) - (twoOmega ^ hu) - g*h*fvc::grad(h);
hu = huOld + offCentre*dt*dhudt;
hu.correctBoundaryConditions();
divUold = fvc::div(U);
//hTotal = h + h0;

