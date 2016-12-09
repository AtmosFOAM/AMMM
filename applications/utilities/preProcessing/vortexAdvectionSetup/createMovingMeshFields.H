// Open control dictionary
IOdictionary monitorDict
(
 IOobject
 (
  "monitorDict", runTime.system(), runTime,
  IOobject::MUST_READ_IF_MODIFIED
  )
 );

// The monitor funciton
autoPtr<monitorFunction> monitorFunc(monitorFunction::New(monitorDict));

const scalar conv = readScalar(monitorDict.lookup("conv"));
const dimensionedScalar Gamma(monitorDict.lookup("Gamma"));


// Create the mesh displacement potential, Phi
volScalarField mmPhi
(
    IOobject("mmPhi",runTime.timeName(),mesh,
             IOobject::MUST_READ, IOobject::AUTO_WRITE),
    mesh
);
mmPhi.oldTime();

// Create the perturbation from Phi
volScalarField mmphi("mmphi", mmPhi);
mmphi == dimensionedScalar("mmphi", dimArea, scalar(0));

// Create phiBarLaplacian
volScalarField mmphiBarLaplacian("mmphiBarLaplacian", fvc::laplacian(mmPhi));

// The gradient of Phi at cell centres
volVectorField gradmmPhi
(
    IOobject("gradmmPhi", runTime.timeName(), mesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    fvc::grad(mmPhi)
);

// The gradient of Phi on faces
surfaceVectorField gradmmPhif
(
    IOobject("gradmmPhif", runTime.timeName(), mesh, 
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    fvc::interpolate(gradmmPhi),
    "calculated"
);
gradmmPhif += (fvc::snGrad(mmPhi) - (gradmmPhif & mesh.Sf())/mesh.magSf())
            *mesh.Sf()/mesh.magSf();

// Hessian of Phi and its determinant
volTensorField Hessian("Hessian", fvc::grad(gradmmPhif));
Hessian*=0;
volScalarField detHess
(
    IOobject("detHess", runTime.timeName(), mesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    mesh,
    dimensionedScalar("detHess", dimless, scalar(1))
);

volScalarField detHessR
(
    IOobject("detHessR", runTime.timeName(), rMesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    rMesh,
    dimensionedScalar("", dimless, scalar(1))
);

// The matrix A
volTensorField matrixA
(
    IOobject("matrixA", runTime.timeName(), mesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    mesh,
    dimensionedTensor("A", dimless, tensor::I)
);
matrixA*=0;
Info << "createMovingMeshFields.H made matrixA finished" << endl;
// The original monitor function on the old mesh
const volScalarField monitor
(
    IOobject("monitor", runTime.timeName(), mesh, IOobject::READ_IF_PRESENT),
    mesh,
    dimensionedScalar("monitor", dimensionSet(0,0,0,0,0), scalar(1)),
    wordList(6,"zeroGradient")
    //
);

// The monitor function on the old mesh calculated from the new mesh
volScalarField monitorNew("monitorNew", monitor);

// The monitor function on the new mesh
volScalarField monitorR
(
    IOobject("monitorR", runTime.timeName(), rMesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    monitorFunc().map(rMesh, monitor)
);

setInternalAndBoundaryValues(monitorNew, monitorR);

// The equidistribution PAB
// The equidistribution
volScalarField equiDist
(
    IOobject("equiDist", runTime.timeName(), rMesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    monitorR*detHess
);

volScalarField PABequi
(
    IOobject("PABequi", runTime.timeName(), rMesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    monitorR*detHess
);

// The global equidistribution
dimensionedScalar Vtot = sum(mesh.V());
dimensionedScalar PABem = fvc::domainIntegrate(equiDist)/Vtot;
dimensionedScalar PABe = sqrt(fvc::domainIntegrate(sqr(equiDist - PABem)))
                         /(Vtot*PABem);

// The mean equidistribution, c
dimensionedScalar equiDistMean = fvc::domainIntegrate(detHess)
                                 /fvc::domainIntegrate(1/monitorNew);

// Source term
volScalarField source
(
    IOobject("source", runTime.timeName(), rMesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    detHess - equiDistMean/monitorNew
);
Info << "createMovingMeshFields.H finished" << endl;

pointField meshUpoints = pMesh.points();


volScalarField monitorP //the monitor function as calculated on the pMesh
(
 IOobject("monitorP", runTime.timeName(), pMesh, IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE),
    pMesh,
 dimensionedScalar("monitorP", dimensionSet(0,0,0,0,0), scalar(1)),
 wordList(6,"zeroGradient")
    //
);

volScalarField monitorM //the monitor function as calculated on the pMesh
(
 IOobject("monitorM", runTime.timeName(), mesh, IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE),
    mesh,
 dimensionedScalar("monitorM", dimensionSet(0,0,0,0,0), scalar(1)),
 wordList(6,"zeroGradient")
    //
);


