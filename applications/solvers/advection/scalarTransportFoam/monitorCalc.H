surfaceVectorField gradT = fvc::interpolate(fvc::grad(T, "gradT"), "gradT");

Info << "Monitor function goes from " << flush;

monitorR = monitorFunc().evaluate(gradT);
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

Info << min(monitor).value() << " to " << max(monitor).value() << endl;
