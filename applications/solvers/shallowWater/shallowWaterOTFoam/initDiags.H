fileName diags = args.rootPath() / args.caseName() / "diags.dat";
OFstream os(diags);
os << "#time   energy normalisedChange momentum normalisedChange" << endl << "# ";

scalar energy0 = 1;
vector momentum0 = vector(1,1,1);
scalar energy = 0;
vector momentum = vector::zero;
scalar energyDiff = 0;
vector momentumDiff = vector::zero;
const scalar Vtot = sum(mesh.V()).value();

#include "diags.H"

energy0 = energy;
momentum0 = momentum;

#include "diags.H"

