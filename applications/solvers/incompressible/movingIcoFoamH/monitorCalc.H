Info << "Monitor function goes from " << flush;

setInternalAndBoundaryValues
(
    monitor,
    sqrt(1 + magSqr(monScale*vorticity))
);

for(int iSmooth = 0; iSmooth < nMonSmooth; iSmooth++)
{
    monitor += 0.25*fvc::laplacian(1/sqr(mesh.deltaCoeffs()), monitor);
}

Info << min(monitor).value() << " to " << max(monitor).value() << endl;
