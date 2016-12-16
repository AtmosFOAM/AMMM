//Info << " Min mmPhi = " << min(mmPhi) << " max mmPhi = " << max(mmPhi) << endl;
mmphi = 0.0*mmPhi;
//Info << " Min mmphi = " << min(mmphi) << " max mmphi = " << max(mmphi) << endl;


mmphiBarLaplacian = fvc::laplacian(mmPhi);
//Info << " Min mmphiBarLaplacian = " << min(mmphiBarLaplacian) << " max mmphiBarLaplacian = " << max(mmphiBarLaplacian) << endl;

gradmmPhi = fvc::grad(mmPhi);

gradmmPhif = fvc::interpolate(gradmmPhi);
gradmmPhif += (fvc::snGrad(mmPhi) - (gradmmPhif & mesh.Sf())/mesh.magSf())
            *mesh.Sf()/mesh.magSf();

// Hessian of Phi and its determinant

Hessian = fvc::grad(gradmmPhif);
forAll(detHess, cellI)
            {
                detHess[cellI] = det(tensor::I + Hessian[cellI]);
            }

setInternalValues(detHessR,detHess);
//Info << " min detHess = " << min(detHess) << " max detHess = " << max(detHess) << endl;
//Info << " min detHessR = " << min(detHessR) << " max detHessR = " << max(detHessR) << endl;

forAll(matrixA, cellI)
            {
                tensor localA (0,0,0,0,0,0,0,0,0);scalar localAew = 0.0;
                matrixA[cellI] = tensor::I*(1+mmphiBarLaplacian[cellI]) - Hessian[cellI];
                //matrixA[cellI].yy() = 1.0;
                localA = matrixA[cellI];
                localAew = eigenValues(localA)[0];
                if(localAew <= 0)
                    {
                        matrixA[cellI] = localA + (1.0e-5 - localAew)*tensor::I;
                    }
                else
                    {
                        matrixA[cellI] = localA;
                    }
            }

//Info << " min monitorR = " << min(monitorR) << " max monitorR = " << max(monitorR) << endl;
equiDist = monitorR*detHess;
//Info << " min equidist = " << min(equiDist) << " max equidist = " << max(equiDist) << endl;

PABequi = monitorR*detHess;
//Info << " min PABequi = " << min(PABequi) << " max PABequi = " << max(PABequi) << endl;

PABem = fvc::domainIntegrate(equiDist)/Vtot;
PABe = sqrt(fvc::domainIntegrate(sqr(equiDist - PABem)))
                         /(Vtot*PABem);

//Info << "calculated PABe = " << PABe << endl;

setInternalValues(monitorNew, monitorR);
//Info << " min monitorNew = " << min(monitorNew) << " max monitorNew = " << max(monitorNew) << endl;
equiDistMean = fvc::domainIntegrate(detHess)
                         /fvc::domainIntegrate(1/monitorNew);
//Info << "calculated equiDistMean = " << equiDistMean << endl;

source = detHess - equiDistMean/monitorNew;
//Info << "min source = " << min(source) << " max source = " << max(source) << endl;

//Info << "setMovingMeshFields.H finished" << endl;

