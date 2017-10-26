fileName diags = args.rootPath() / args.caseName() / "diagnostics.dat";
OFstream os(diags);
os << "#time   Vtotal   ATtotal   globalSum   normlalisedMassChange" << endl;

const scalar AVT0 = fvc::domainIntegrate(A*T).value();
const scalar Vtot0 = gSum(pMesh.V());

#include "diagnostics.H"

