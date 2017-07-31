surfaceVectorField gradT = fvc::interpolate(fvc::grad(T, "gradT"), "gradT");

Info << "Monitor function goes from " << flush;

monitorP = monitorFunc().evaluate(gradT);
setInternalAndBoundaryValues(monitor, monitorP);

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

setInternalAndBoundaryValues(monitorP, monitor);

Info << min(monitor).value() << " to " << max(monitor).value() << endl;
