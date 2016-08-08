// Create the mesh displacement potential, Phi
volScalarField Phi
(
    IOobject("Phi",runTime.timeName(),mesh,
             IOobject::MUST_READ, IOobject::AUTO_WRITE),
    mesh
);
Phi.oldTime();

// The gradient of Phi at cell centres
volVectorField gradPhi
(
    IOobject("gradPhi", runTime.timeName(), mesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    fvc::grad(Phi)
);
gradPhi.oldTime();

// The gradient of Phi on faces
surfaceVectorField gradPhif
(
    IOobject("gradPhif", runTime.timeName(), mesh, 
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    fvc::interpolate(gradPhi),
    "calculated"
);
gradPhif += (fvc::snGrad(Phi) - (gradPhif & mesh.Sf())/mesh.magSf())
            *mesh.Sf()/mesh.magSf();

// Hessian of Phi and its determinant
volTensorField Hessian("Hessian", fvc::grad(gradPhif));
volScalarField detHess
(
    IOobject("detHess", runTime.timeName(), mesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    mesh,
    dimensionedScalar("detHess", dimless, scalar(1))
);

// The original monitor function on the old mesh
const volScalarField monitor
(
    IOobject("monitor", runTime.timeName(), mesh, IOobject::READ_IF_PRESENT),
    mesh,
    dimensionedScalar("monitor", dimensionSet(0,0,0,0,0), scalar(1)),
    wordList(5,"zeroGradient")
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
volScalarField PABequi
(
    IOobject("PABequi", runTime.timeName(), rMesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    monitorR*detHess
);


// The equidistribution
//volScalarField equiDist("equiDist", monitorR*detHess);
volScalarField equiDist
(
    IOobject("equiDist", runTime.timeName(), rMesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    monitorR*detHess
);

// The global equidistribution
dimensionedScalar Vtot = sum(mesh.V());
dimensionedScalar PABem = fvc::domainIntegrate(equiDist)/Vtot;
dimensionedScalar PABe = sqrt(fvc::domainIntegrate(sqr(equiDist - PABem)))
                         /(Vtot*PABem);



dimensionedScalar constant = fvc::domainIntegrate(pow(detHess*monitorNew,0.5))/Vtot;

// Source term
volScalarField source
(
    IOobject("source", runTime.timeName(), rMesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    mesh,
    dimensionedScalar("source", dimensionSet(0,0,0,0,0), scalar(0)),
    wordList(5,"zeroGradient")
    //    pow(detHess*monitorNew, 0.5) - constant
);

dimensionedScalar equiDistMean = fvc::domainIntegrate(detHess)
    /fvc::domainIntegrate(1/monitorNew);

dimensionedScalar rdt = 1/runTime.deltaT();



Info << "Dimensions of Phi: " << Phi.dimensions() << endl;
//Info << "Dimensions of equiDistMean: " << equiDistMean.dimensions() << endl;
Info << "Dimensions of gradPhi: " << gradPhi.dimensions() << endl;
Info << "Dimensions of gradPhif: " << gradPhif.dimensions() << endl;
Info << "Dimensions of Hessian: " << Hessian.dimensions() << endl;
Info << "Dimensions of detHess: " << detHess.dimensions() << endl;
Info << "Dimensions of monitor: " << monitor.dimensions() << endl;
Info << "Dimensions of monitorNew: " << monitorNew.dimensions() << endl;
Info << "Dimension of the integral 1/monitorNew: " << fvc::domainIntegrate(1/monitorNew).dimensions() << endl;
Info << "Dimension of the integral of detHess: " << fvc::domainIntegrate(detHess).dimensions() << endl;
Info << "Dimensions of source: " << source.dimensions() << endl;
