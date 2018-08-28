Info<< "Reading field h\n" << endl;
volScalarField h
(
    IOobject
    (
        "h",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info << "Reading mountain height, h0\n" << endl;
volScalarField h0
(
    IOobject
    (
        "h0",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    mesh,
    dimensionedScalar("h0", dimLength, scalar(0))
);

surfaceVectorField Uf
(
    IOobject("Uf", runTime.timeName(), mesh),
    fvc::interpolate(U)
);
Uf.oldTime();
Uf.write();

surfaceScalarField hf("hf", fvc::interpolate(h));

surfaceVectorField hUf
(
    IOobject("hUf", runTime.timeName(), mesh),
    hf*Uf
);

surfaceScalarField phi
(
    IOobject
    (
        "phi", runTime.timeName(), mesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    hf*(fvc::interpolate(U) & mesh.Sf())
);

