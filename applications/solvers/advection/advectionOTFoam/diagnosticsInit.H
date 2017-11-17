fileName diags = args.rootPath() / args.caseName() / "diagnostics.dat";
OFstream os(diags);
os << "#time   normalisedVolChange   normalisedATChange   normalisedTChange"
   << endl;

const scalar V0 = gSum(mesh.V());
const scalar AT0 = fvc::domainIntegrate(A*T).value();
const scalar T0 = fvc::domainIntegrate(T).value();

#include "diagnostics.H"
