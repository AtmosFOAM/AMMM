Info << "Monitor function goes from " << flush;

monitorR = monitorFunc().evaluate(Uf);
setInternalAndBoundaryValues(monitor, monitorR);

Info << min(monitor).value() << " to " << max(monitor).value() << endl;
