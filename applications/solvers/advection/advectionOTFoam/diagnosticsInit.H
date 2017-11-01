fileName diags = args.rootPath() / args.caseName() / "diagnostics.dat";
fileName volchange = args.rootPath() / args.caseName() / "normalisedVolChange.dat";
fileName ATchange = args.rootPath() / args.caseName() / "normalisedATChange.dat";
fileName Tchange = args.rootPath() / args.caseName() / "normalisedTChange.dat";
OFstream os(diags);
os << "#time   Vtotal   ATtotal   Ttotal   globalSum" << endl;

OFstream os_vol(volchange);
os_vol << "normalisedVolChange" << endl;

OFstream os_AT(ATchange);
os_AT << "normalisedATChange" << endl;

OFstream os_T(Tchange);
os_T << "normalisedTChange" << endl;

const scalar V0 = gSum(mesh.V());
const scalar AT0 = fvc::domainIntegrate(A*T).value();
const scalar T0 = fvc::domainIntegrate(T).value();

#include "diagnostics.H"
