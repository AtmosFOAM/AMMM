// 1. Move the mesh
Info << " in pEqn.H " << endl;
pMesh.movePoints(meshUpoints);
Info << " I have just done pMesh.movePOints " << endl;
Info << " The divergence of u = " << min(div) << " to " << max(div) << endl;
{
for (int corr=0; corr<nCorr; corr++)
    {

        for (int nonOrth=0; nonOrth<=nNonOrthCorr; nonOrth++)
            {

        
        // 5. Make U relative:
        fvc::makeRelative(phi,u);
        #include "CourantNo.H"
        Info << "before fvVectorMatrix " << max(u) << endl;
        fvVectorMatrix uEqn
            (
             fvm::ddt(u) + (
                            (fvc::div(phi,u))
              //+ (2*(Omega^(Uf-Ug)))
              )
             );
        uEqn.solve();
        Info << "after fvVectorMatrix " << max(u) << endl;

        
                phi = fvc::interpolate(u) & pMesh.Sf();
                Info << " before pEqn " << max(phi) << endl;
                fvScalarMatrix pEqn
                    (
                     fvc::div(u)
                     - dt*fvm::laplacian(p)
                     );
                if (setReferenceP) pEqn.setReference(0,0);
                
                if(nonOrth == nNonOrthCorr && corr == nCorr-1)
                    {
                        Info << "pFinal ";
                pEqn.solve(pMesh.solver(p.name() + "Final"));
                    }
                else
                    {
                pEqn.solve(pMesh.solver(p.name()));
                    }
     
                
                //  if (nonOrth == nNonOrthCorr)
                // {
                    //Ucor = -dt*fvc::snGrad(p)*pMesh.magSf();
                    //phi += pEqn.flux();///offCentre;
                    //U += Ucor;
                    //U -= dt*offCentre*fvc::snGrad(p)*pMesh.magSf();
                    //                    V -= dt*offCentre*fvc::snGrad(p)*H.magd();
                div = fvc::div(u);
                Info << " The divergence of u before adding grad p = " << min(div) << " to " << max(div) << endl;
                                    
                    u -= dt*fvc::grad(p);
                    phi = fvc::interpolate(u) & pMesh.Sf();
                    div = fvc::div(u);
                    Info << " The divergence of u after  adding grad p = " << min(div) << " to " << max(div) << endl;
                    //u = H.reconstructd(V);
                    //Uf = fvc::interpolate(u);
                    //Ufv = Uf - ((Uf & H.delta()) - V)*H.delta()/magSqr(H.delta());
                    q = fvc::curl(u);
                    // }
            }
    }
}


// Updates for next time step
//dVdt -= H.magd()*fvc::snGrad(p);


