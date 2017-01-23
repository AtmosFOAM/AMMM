Info << "Reading in monitor fucntion controls from monitorFromGradU" << endl;

IOdictionary monDict
(
    IOobject
    (
        "monitorFromGradU", mesh.time().system(), mesh,
        IOobject::MUST_READ, IOobject::NO_WRITE
    )
);

const dimensionedScalar monScale(monDict.lookup("monScale"));

// The monitor function on the computational mesh
volScalarField monitor
(
    IOobject("monitor", runTime.timeName(), mesh),
    mesh,
    dimensionedScalar("monitor", dimless, scalar(1))
);
// Set monitor function from the moving mesh
Info << "Monitor function goes from " << flush;
setInternalAndBoundaryValues
(
   monitor,
   sqrt(monScale*magSqr(fvc::grad(Uf)) + 1)
);
Info << min(monitor).value() << " to " << max(monitor).value() << endl;

Info << "Creating in the mesh displacement potential, mmPhi" << endl;
volScalarField mmPhi
(
    IOobject("mmPhi",runTime.timeName(), mesh),
    mesh,
    dimensionedScalar("mmPhi", dimArea, scalar(0)),
    "zeroGradient"
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

