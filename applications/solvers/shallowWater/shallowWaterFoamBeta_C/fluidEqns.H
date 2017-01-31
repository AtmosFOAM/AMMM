// Extra geometry for Hodge operators on the new mesh
HodgeOps H(rMesh);
faceVol = 1/3.*(H.delta() & rMesh.Sf());
meshPhi = fvc::meshPhi(h, u);

// Calculate old V on the new mesh, including updates from old time-level
V0 = faceVol.oldTime()/faceVol*
(
    (H.delta() & hUf) + (1-offCentre)*dt*dVdt.oldTime()
);

for (int corr=0; corr<nCorr; corr++)
{
    // Make the flux relative
    U -= meshPhi;
    #include "CourantNo.H"
    
    // Explicit part of momentum without Coriolis (or pressure gradient)
    dVdt = -H.delta() & fvc::interpolate(fvc::div(U, u), "Uf");
    V = V0 + offCentre*dt*dVdt;
    
    // Solve pressure equation
    for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
    {
        // In this loop, U is NOT relative
        U = H.ddirToFlux(V - offCentre*dt*(two_dxOmega & hUf))
          - H.ddirToFluxCorr(dt*g*hf*fvc::snGrad(h)*H.magd());
        fvScalarMatrix hEqn
        (
            fvm::ddt(h)
          - fvc::div(meshPhi)
          + (1-offCentre)*divUold
          + offCentre*fvc::div(U)
          - fvm::laplacian(dt*sqr(offCentre)*g*hf, h, "h")
        );

        if(nonOrth == nNonOrthCorr && corr == nCorr-1)
        {
            hEqn.solve(rMesh.solver(h.name() + "Final"));
        }
        else
        {
            hEqn.solve(rMesh.solver(h.name()));
        }

        if (nonOrth == nNonOrthCorr)
        {
            U += hEqn.flux()/offCentre;
            V -= offCentre*dt*(two_dxOmega & hUf)
               + offCentre*dt*g*hf*fvc::snGrad(h)*H.magd();
            u = H.reconstructd(V)/h;
            hUf = fvc::interpolate(H.reconstructd(V), "Uf");
            Uf = fvc::interpolate(u, "Uf");
            hf = fvc::interpolate(h);
        }
    }
}

// Updates for next time step
hUf = hUf - ((hUf & H.delta()) - V)*H.delta()/magSqr(H.delta());
dVdt = -(H.delta() & fvc::interpolate(fvc::div(U-meshPhi, u, "div(U,u)"), "Uf"))
     - (two_dxOmega & hUf)
     - g*hf*fvc::snGrad(h)*H.magd();
divUold = fvc::div(U);
//hTotal = h + h0;

