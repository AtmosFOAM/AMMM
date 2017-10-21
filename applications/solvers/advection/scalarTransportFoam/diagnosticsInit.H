fileName diags = args.rootPath() / args.caseName() / "diagnostics.dat";
OFstream os(diags);
os << "#time   normlalisedMassChange" << endl;

const scalar AVT0 = fvc::domainIntegrate(A*T).value();

#include "diagnostics.H"

