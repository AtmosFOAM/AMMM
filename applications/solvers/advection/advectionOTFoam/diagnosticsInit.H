fileName diags = args.rootPath() / args.caseName() / "diagnostics.dat";
OFstream os(diags);
os << "#time   normalisedVolumeChange   Ttotal   globalSum   normlalisedMassChange" << endl;

const scalar VT0 = fvc::domainIntegrate(T).value();
const scalar Vtot0 = gSum(mesh.V());

#include "diagnostics.H"
