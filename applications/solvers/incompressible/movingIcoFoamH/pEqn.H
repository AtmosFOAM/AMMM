// Extra geometry for Hodge operators on the new mesh
HodgeOps H(rMesh);

// Calculate old V on the new mesh, V0 = u^n \cdot d^{n+1}
V0 = H.delta() & Ufv;

// Add explicit part
V0 += -(1-offCentre)*dt*
(
    H.delta() &
    (
        fvc::interpolate(fvc::div(U.oldTime()-meshPhi.oldTime(),u.oldTime()))
        //+ (2*(Omega^(Uf-Ug)))
    )
);

meshPhi = fvc::meshPhi(u);

for (int corr=0; corr<nCorr; corr++)
{
    // Make U relative:
    U -= meshPhi;
    #include "CourantNo.H"
    
    // Explicit part of momentum
    V = V0 - offCentre*dt*(H.delta() & fvc::interpolate(fvc::div(U,u)));
    
    // Solve pressure equation to project V and U to be divergence free
    for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
    {
        U = H.ddirToFlux(V)
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
            u = H.reconstructd(V);
        }
    }
}

// Updates for next time step
Uf = fvc::interpolate(u);
Ufv = Uf - ((Uf & H.delta()) - V)*H.delta()/magSqr(H.delta());
 
