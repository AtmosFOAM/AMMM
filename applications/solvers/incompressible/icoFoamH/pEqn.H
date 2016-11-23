// update old time variables for Crank-Nicholson
V.oldTime() += (1-offCentre)*dt*dVdt; //this is on the old mesh no worries

// Initial prediction of cell centre velocity without a projection step
// for the initialcalculation of the non-linear advection term
if (uPredictor)
{
    Info << "U predictor" << endl;
    // Predictor values of V, Uf and u
    V = V.oldTime() - offCentre*dt*
    (
        (H.delta() &
        (
            fvc::interpolate(fvc::div(U,u))
          //+ (2*(Omega^(Uf-Ug)))
        ))
      + fvc::snGrad(p)*H.magd()
    );
    u = H.reconstructd(V);
    Uf = fvc::interpolate(u);
 } //I can ignore this uPredictor loop apparently


{
    for (int corr=0; corr<nCorr; corr++)
    {
        // Explicit part of rate of change of flux
        dVdt = -H.delta() &
        (
            fvc::interpolate(fvc::div(U,u))
         //+ (2*(Omega^(Uf-Ug)))
        );

        // the flux with grad(p) (to get the BCs right)
        V = V.oldTime() + offCentre*dt*dVdt;

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
                Ucor = -dt*offCentre*fvc::snGrad(p)*pMesh.magSf();
                //U += pEqn.flux();
                U += Ucor;
                //U -= dt*offCentre*fvc::snGrad(p)*pMesh.magSf();
                V -= dt*offCentre*fvc::snGrad(p)*H.magd();
                u = H.reconstructd(V);
                Uf = fvc::interpolate(u);
            }
        }
    }
}


// Updates for next time step
dVdt -= H.magd()*fvc::snGrad(p);
