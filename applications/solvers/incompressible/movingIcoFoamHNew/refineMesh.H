if (MArefinement) {

    bool converged = false;
    Info << "runtime.value() = " << runTime.value() << endl;
    if ((runTime.value() == 0) && (PABe.value() < conv) ) {

        Info << "Iterations = 0 PABe = " << PABe << endl;
        converged = true;
    }
    
    scalar iterations = 0;
    while ( not converged )
        {
            mmphiBarLaplacian = fvc::laplacian(mmPhi);
            tensor localA (0,0,0,0,0,0,0,0,0);
            scalar localAew = 0.0;
            bool disp=false;
            forAll(matrixA, cellI)
                {
            
                    matrixA[cellI] = tensor::I*(1+mmphiBarLaplacian[cellI]) - Hessian[cellI];
                    //matrixA[cellI].yy() = 1.0;
                    localA = matrixA[cellI];
                    localAew = eigenValues(localA)[0];
                    if(localAew <= 0)
                        {
                            if(disp)
                                {
                                    Info << "Minimum eigenvalue = " << localAew << endl;
                                    disp = false;
                                }
                            matrixA[cellI] = localA + (1.0e-5 - localAew)*tensor::I;
                        }
                    else
                        {
                            matrixA[cellI] = localA;
                        }
                }
            // Calculate the source terms for the MA equation
            source = detHess - equiDistMean/monitorNew;

            // Setup and solve the MA equation to find Psi(t+1) 
            for (int iCorr = 0; iCorr < nCorr; iCorr++)
                {
                    fvScalarMatrix mmPhiEqn
                        (
                         Gamma*fvm::laplacian(matrixA, mmphi)
                         + source
                         );
                    mmPhiEqn.setReference(0, scalar(0));
                    solverPerformance::debug=0;
                    mmPhiEqn.solve();            
                    solverPerformance::debug=1;
                }
        
            mmPhi += mmphi;
            mmphi == dimensionedScalar("mmphi", dimArea, scalar(0));

            // Calculate the gradient of Phi at cell centres and on faces
            gradmmPhi = fvc::grad(mmPhi);

            // Interpolate gradPhi (gradient of Phi) onto faces and correct the normal component
            gradmmPhif = fvc::interpolate(gradmmPhi);
            gradmmPhif += (fvc::snGrad(mmPhi) - (gradmmPhif & mesh.Sf())/mesh.magSf())
                *mesh.Sf()/mesh.magSf();

            // Map gradPhi onto vertices in order to create the new mesh
            pointVectorField gradmmPhiP
                = fvc::faceToPointReconstruct(fvc::snGrad(mmPhi));
        
            rMesh.movePoints(mesh.points() + gradmmPhiP);
        
            // finite difference Hessian of phiBar and its determinant
            Hessian = fvc::grad(gradmmPhif);
            forAll(detHess, cellI)
                {
                    detHess[cellI] = det(tensor::I + Hessian[cellI]);
                }

            // map to or calculate the monitor function on the new mesh
            meshToMesh0 meshToMesh0Interp(mesh,rMesh);
            meshToMesh0::order mapOrder = meshToMesh0::INTERPOLATE;
            meshToMesh0Interp.interpolate(monitorR,monitorM,mapOrder,eqOp<scalar>());
        
            //monitorR = monitorFunc().map(rMesh, monitor);
            setInternalValues(monitorNew, monitorR);

            // The Equidistribution
            equiDist = monitorR*detHess;

            // mean equidistribution, c
            equiDistMean = fvc::domainIntegrate(detHess)
                /fvc::domainIntegrate(1/monitorNew);

            // The global equidistribution as CV of equidistribution
            PABem = fvc::domainIntegrate(equiDist)/Vtot;
            PABe = sqrt(fvc::domainIntegrate(sqr(equiDist - PABem)))/(Vtot*PABem);
            Info << "Iterations = " << iterations << " PABe = " << PABe.value() << endl;
            converged = PABe.value() < conv || iterations > maxIters;
            //Info << "convergence criteria PABe = " << PABe.value() << endl; 
            if (iterations == 0) {
                Info << "refineMesh convergence criteria start  = " << PABe.value() << endl;


//                 Info << " Min mmPhi = " << min(mmPhi) << " max mmPhi = " << max(mmPhi) << endl;
// Info << " Min mmphi = " << min(mmphi) << " max mmphi = " << max(mmphi) << endl;
// Info << " Min mmphiBarLaplacian = " << min(mmphiBarLaplacian) << " max mmphiBarLaplacian = " << max(mmphiBarLaplacian) << endl;
// Info << " min detHess = " << min(detHess) << " max detHess = " << max(detHess) << endl;
// Info << " min detHessR = " << min(detHessR) << " max detHessR = " << max(detHessR) << endl;
// Info << " min monitorR = " << min(monitorR) << " max monitorR = " << max(monitorR) << endl;
// Info << " min equidist = " << min(equiDist) << " max equidist = " << max(equiDist) << endl;
//    Info << " min PABequi = " << min(PABequi) << " max PABequi = " << max(PABequi) << endl;
//  Info << "calculated PABe = " << PABe << endl;
//  Info << " min monitorNew = " << min(monitorNew) << " max monitorNew = " << max(monitorNew) << endl;
// Info << "calculated equiDistMean = " << equiDistMean << endl;
// Info << "min source = " << min(source) << " max source = " << max(source) << endl;

                
            }
            if (converged)
                {
                    Info << "refineMesh iterations = " << iterations << endl;
                    Info << "refineMesh convergence criteria finish = " << PABe.value() << endl;
                    Info << "refineMesh ExecutionTime = " << runTime.elapsedCpuTime() << " s"
                         << endl;
                }
            iterations++;
        }

 } else {
    
    forAll(meshUpoints, point)
        {
            scalar x_orig = mesh.points()[point].x();
            // scalar theta_orig = Foam::atan2
            // (
            //     mesh.points()[point].y(), mesh.points()[point].x()
            // );
            // scalar r_orig = Foam::sqrt
            // (
            //     sqr(mesh.points()[point].x()) + sqr(mesh.points()[point].y())
            // );
            scalar x_new = x_orig + (dx/10)*Foam::sin(tau*x_orig/dx)
                *Foam::sin(
                           tau*runTime.value()/meshPeriod.value()
                           )*tau/16;
            // scalar theta_new = theta_orig
            //                  + Foam::sin(theta_orig)
            //                  *Foam::sin
            //                  (
            //                     tau*runTime.value()/meshPeriod.value()
            //                  )*tau/16;

            meshUpoints[point].x() = x_new;
            meshUpoints[point].y() = rMesh.points()[point].y();
            meshUpoints[point].z() = rMesh.points()[point].z();
        }
    rMesh.movePoints(meshUpoints);

 }
rMesh.write();
