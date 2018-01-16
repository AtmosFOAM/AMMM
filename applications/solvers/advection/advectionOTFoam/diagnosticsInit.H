fileName diags = args.rootPath() / args.caseName() / "diagnostics.dat";

OFstream os(diags);
os << "#time   totalV   totalAV   Vchange   AVchange   Tchange   ATchange   uniTchange   AuniTchange"
   << endl;

const scalar V0 = gSum(mesh.V());
const scalar AV0 = fvc::domainIntegrate(A).value();
const scalar T0 = fvc::domainIntegrate(T).value();
const scalar AT0 = fvc::domainIntegrate(A*T).value();
const scalar uniT0 = fvc::domainIntegrate(uniT).value();
const scalar AuniT0 = fvc::domainIntegrate(A*uniT).value();

#include "diagnostics.H"
