Info<< "Reading field T\n" << endl;

volScalarField T
(
    IOobject
    (
        "T",
        runTime.timeName(),
        pMesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
        ),
    pMesh
);

Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        pMesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
        ),
    pMesh
);
Info<< "Creating field phi\n" << endl;
surfaceScalarField phi
(
    IOobject
    (
        "phi",
        runTime.timeName(),
        pMesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
        ),
    linearInterpolate(U) & pMesh.Sf()
);

pointField meshUpoints = pMesh.points();

