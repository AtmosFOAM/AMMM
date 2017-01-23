Info << "Monitor function goes from " << flush;

setInternalAndBoundaryValues
(
    monitor,
    sqrt(monScale*magSqr(fvc::grad(Uf)) + 1)
);

Info << min(monitor).value() << " to " << max(monitor).value() << endl;
