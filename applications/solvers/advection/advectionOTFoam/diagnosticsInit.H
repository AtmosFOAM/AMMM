fileName diags = args.rootPath() / args.caseName() / "diagnostics.dat";

OFstream os(diags);
os << "#time   totalV   totalAV   Vchange   AVchange   Tchange   ATchange"
   << endl;

const scalar V0 = gSum(mesh.V());
const scalar AV0 = fvc::domainIntegrate(A).value();
const scalar T0 = fvc::domainIntegrate(T).value();
const scalar AT0 = fvc::domainIntegrate(A*T).value();

#include "diagnostics.H"
