Info << "Reading in monitor fucntion controls from initDict" << endl;

IOdictionary monDict
(
    IOobject("initDict", mesh.time().system(), mesh, IOobject::MUST_READ)
);

// The monitor funciton
autoPtr<monitorFunctionFrom> monitorFunc(monitorFunctionFrom::New(monDict));

// The monitor function on both meshes
volScalarField monitor
(
    IOobject("monitor", runTime.timeName(), mesh),
    mesh,
    dimensionedScalar("monitor", dimless, scalar(1))
);
volScalarField monitorR
(
    IOobject
    (
        "monitor", runTime.timeName(), rMesh,
        IOobject::NO_READ, IOobject::AUTO_WRITE
    ),
    rMesh,
    dimensionedScalar("monitor", dimless, scalar(1))
);
#include "monitorCalc.H"
monitorR.write();

Info << "Creating in the mesh displacement potential, mmPhi" << endl;
volScalarField mmPhi
(
    IOobject
    (
        "mmPhi",runTime.timeName(), mesh,
        IOobject::MUST_READ, IOobject::AUTO_WRITE
    ),
    mesh
);

// The gradient of Phi on faces
surfaceVectorField gradPhif("gradPhif", fvc::interpolate(fvc::grad(mmPhi)));
gradPhif += (fvc::snGrad(mmPhi) - (gradPhif & mesh.Sf())/mesh.magSf())
            *mesh.Sf()/mesh.magSf();

// Hessian of Phi and its determinant
volTensorField Hessian("Hessian", fvc::grad(gradPhif));
volScalarField detHess("detHess", fvc::det(Hessian + tensor::I));

// The matrix A of co-factors
volTensorField cofacA("cofacA", fvc::posDefCof(Hessian + tensor::I));

// The mean equidistribution, c
dimensionedScalar equiDistMean = fvc::domainIntegrate(detHess)
                                 /fvc::domainIntegrate(1/monitor);

// Source term
volScalarField source("source", detHess - equiDistMean/monitor);

// The laplacian of (A,mmPhi) at the old time
volScalarField laplacianAPhi("laplacianAPhi", fvc::laplacian(cofacA,mmPhi));
