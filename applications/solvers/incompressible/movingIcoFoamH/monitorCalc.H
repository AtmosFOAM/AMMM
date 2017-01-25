Info << "Monitor function goes from " << flush;

setInternalAndBoundaryValues
(
    monitor,
    sqrt(monScale*magSqr(fvc::grad(Uf)) + 1)
);

for(int iSmooth = 0; iSmooth < nMonSmooth; iSmooth++)
{
    monitor += 0.25*fvc::laplacian(1/sqr(mesh.deltaCoeffs()), monitor);
}

Info << min(monitor).value() << " to " << max(monitor).value() << endl;
