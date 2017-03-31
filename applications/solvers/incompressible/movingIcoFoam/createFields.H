Info<< "Reading transportProperties\n" << endl;

Info<< "Reading field p\n" << endl;
volScalarField p
(
    IOobject
    (
        "p",
        runTime.timeName(),
        rMesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    rMesh
);


Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        rMesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    rMesh
);

surfaceVectorField Uf
(
    IOobject("Uf", runTime.timeName(), rMesh),
    fvc::interpolate(U)
);

surfaceScalarField phi
(
    IOobject
    (
        "phi", runTime.timeName(), rMesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    Uf & rMesh.Sf()
);

label pRefCell = 0;
scalar pRefValue = 0.0;
setRefCell(p, rMesh.solutionDict().subDict("PISO"), pRefCell, pRefValue);
rMesh.setFluxRequired(p.name());

//IOMRFZoneList MRF(rMesh);

