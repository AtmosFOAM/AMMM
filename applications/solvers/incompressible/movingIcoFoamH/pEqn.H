// 1. Move the mesh
pMesh.movePoints(meshUpoints);

// 2. Make the boundaries orthogonal and calculate d^{n+1}
#include "orthogonalBoundariesPMesh.H"
//HodgeOps H(pMesh);

// 3. Calculate V0 = u^n \cdot d^{n+1}
V0 = H.delta() & Uf;
V0 = V.oldTime();

// 3b. Add explicit part
V0 += (1-offCentre)*dt*dVdt;

// 4. Save the meshPhi
meshPhi = fvc::meshPhi(u);

{
for (int corr=0; corr<nCorr; corr++)
    {

        // 5. Make U relative:
        U -= meshPhi;
        //fvc::makeRelative(U,u);
        
        // Explicit part of rate of change of flux
        // Subtract from V0 the term \alpha{\delta t} \div{ u^{n}_r u^{n} } \cdot d^{n+1}
        dVdt = -H.delta() &
        (
            fvc::interpolate(fvc::div(U,u))
            //+ (2*(Omega^(Uf-Ug)))
        );

        V = V0 + offCentre*dt*dVdt;
        
        for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
            {
                U = H.ddirToFlux(V)
                    - H.ddirToFluxCorr(offCentre*dt*fvc::snGrad(p)*H.magd());

                fvScalarMatrix pEqn
                    (
                     fvc::div(U)
                     - fvm::laplacian(offCentre*dt, p)
                     );
                if (setReferenceP) pEqn.setReference(0,0);

                if(nonOrth == nNonOrthCorr && corr == nCorr-1)
                    {
                pEqn.solve(pMesh.solver(p.name() + "Final"));
                    }
                else
                    {
                pEqn.solve(pMesh.solver(p.name()));
                    }

                if (nonOrth == nNonOrthCorr)
                {
                    //Ucor = -dt*fvc::snGrad(p)*pMesh.magSf();
                    U += pEqn.flux()/offCentre;
                    //U += Ucor;
                    //U -= dt*offCentre*fvc::snGrad(p)*pMesh.magSf();
                    V -= dt*offCentre*fvc::snGrad(p)*H.magd();
                    u = H.reconstructd(V);
                    Uf = fvc::interpolate(u);
                    q = fvc::curl(u);
                }
            }
    }
}


// Updates for next time step
dVdt -= H.magd()*fvc::snGrad(p);
