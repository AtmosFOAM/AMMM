bool converged = false;
for(int nMAit = 0; nMAit < 10 && !converged; nMAit++)
{
    // mean equidistribution, c
    equiDistMean = fvc::domainIntegrate(detHess)
        /fvc::domainIntegrate(1/monitor);

    // The matrix of co-factors for linearising the MA eqn
    cofacA = fvc::posDefCof(Hessian + tensor::I);

    // Calculate the source terms for the MA equation
    source = detHess - equiDistMean/monitor;

    // Setup and solve the MA equation to find Psi(t+1) 
    laplacianAPhi = fvc::laplacian(cofacA, mmPhi);
    for (int iCorr = 0; iCorr < 2; iCorr++)
    {
        fvScalarMatrix mmPhiEqn
        (
            fvm::laplacian(cofacA, mmPhi)
          - laplacianAPhi
          + source
        );
        mmPhiEqn.setReference(0, scalar(0));
        solverPerformance sp = mmPhiEqn.solve();
        if (iCorr == 0) converged = sp.nIterations() <= 1;
    }

    // Calculate gradient of Phi on faces and correct the normal component
    gradPhif = fvc::interpolate(fvc::grad(mmPhi));
    gradPhif += (fvc::snGrad(mmPhi) - (gradPhif & mesh.Sf())/mesh.magSf())
                *mesh.Sf()/mesh.magSf();

    // Update Hessian and its determinant
    Hessian = fvc::grad(gradPhif);
    detHess = fvc::det(Hessian + tensor::I);
}

// Map gradPhi onto vertices in order to create the new mesh
pointVectorField gradPhiP = fvc::faceToPointReconstruct(fvc::snGrad(mmPhi));
rMesh.movePoints(mesh.points() + gradPhiP);

