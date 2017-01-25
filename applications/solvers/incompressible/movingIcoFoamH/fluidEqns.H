// Extra geometry for Hodge operators on the new mesh
HodgeOps H(rMesh);
faceVol = 1/3.*(H.delta() & rMesh.Sf());
meshPhi = fvc::meshPhi(u);

// Calculate old V on the new mesh, including updates from old time-level
V0 = faceVol.oldTime()/faceVol*
(
    (H.delta() & Ufv) + (1-offCentre)*dt*dVdt.oldTime()
);


for (int corr=0; corr<nCorr; corr++)
{
    // Make the flux relative
    U -= meshPhi;
    #include "CourantNo.H"
    
    // Explicit part of momentum without Coriolis (or pressure gradient)
    dVdt = -H.delta() & fvc::interpolate(fvc::div(U, u));
    V = V0 + offCentre*dt*dVdt;
    
    // Solve pressure equation to project V and U to be divergence free
    for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
    {
        // In this loop, U is NOT relative
        U = H.ddirToFlux(V)
          //- offCentre*dt*2*(SfxOmega & Uf)
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
            V -= dt*fvc::snGrad(p)*H.magd();
               //+ ((offCentre*dt*2*(Omega ^ Uf)) & H.delta());
            u = H.reconstructd(V);
            //Uf = fvc::interpolate(u);
        }
    }
}

// Updates for next time step
Uf = fvc::interpolate(u);
Ufv = Uf - ((Uf & H.delta()) - V)*H.delta()/magSqr(H.delta());
dVdt = -H.delta() & 
(
    fvc::interpolate(fvc::div(U-meshPhi, u, "div(U,u)"))
  //+ 2*(Omega^(Uf - Ug))
);
