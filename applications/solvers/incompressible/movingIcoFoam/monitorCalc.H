Info << "Monitor function goes from " << flush;

monitorR = monitorFunc().evaluate(Uf);
setInternalAndBoundaryValues(monitor, monitorR);

// Smoothing of the monitor function on the computational mesh
for(int iSmooth = 0; iSmooth < nMonSmooth; iSmooth++)
{
    monitor += 0.25*fvc::laplacian
    (
        1/sqr(mesh.deltaCoeffs()),
        monitor,
        "laplacian(monitor)"
    );
}

setInternalAndBoundaryValues(monitorR, monitor);

Info << min(monitor).value() << " to " << max(monitor).value() << endl;

