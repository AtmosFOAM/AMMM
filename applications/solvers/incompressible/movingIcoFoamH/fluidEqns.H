// Calculate old V on the new mesh, including updates from old time-level
V0 = faceVol.oldTime()/faceVol*
(
    (H.delta() & Ufv) + (1-offCentre)*dt*dVdt.oldTime()
);

for (int corr=0; corr<nCorr; corr++)
{
    // Make the flux relative
    fvc::makeRelative(U, u);
    if (corr == 0)
    {
        #include "CourantNo.H"
    }
    
    // Explicit part of momentum without Coriolis (or pressure gradient)
    dVdt = -H.delta() & fvc::interpolate(fvc::div(U, u), "divuu");
    V = V0 + offCentre*dt*dVdt;
    
    // Solve pressure equation
    for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
    {
        // In this loop, U is NOT relative
        U = H.ddirToFlux(V - offCentre*dt*(two_dxOmega & (Uf-Ug)))
          - H.ddirToFluxCorr(dt*fvc::snGrad(p)*H.magd());
        fvScalarMatrix pEqn
        (
            fvc::div(U)
          - fvm::laplacian(dt, p)
        );
        if (setReferenceP) pEqn.setReference(0,0);

        if(nonOrth == nNonOrthCorr && corr == nCorr-1)
        {
            pEqn.solve(rMesh.solver(p.name() + "Final"));
        }
        else
        {
            pEqn.solve(rMesh.solver(p.name()));
        }

        if (nonOrth == nNonOrthCorr)
        {
            U += pEqn.flux();
            V -= offCentre*dt*(two_dxOmega & (Uf - Ug))
               + dt*fvc::snGrad(p)*H.magd();
            u = H.reconstructd(V);
            Uf = fvc::interpolate(u, "Uf");
        }
    }
}

// Updates for next time step
Ufv = Uf - ((Uf & H.delta()) - V)*H.delta()/magSqr(H.delta());
dVdt =
   -(
        H.delta() & fvc::interpolate
       (
           fvc::div(fvc::relative(U,u), u, "div(U,u)"),
           "divuu"
       )
    )
  - (two_dxOmega & (Uf - Ug));

