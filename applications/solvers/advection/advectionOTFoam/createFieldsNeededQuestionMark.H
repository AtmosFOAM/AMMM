Info << nl << "Reading const nIteration" << flush;
const label nIteration
(
    readLabel(mesh.solutionDict().lookup("nIteration"))
);
Info<< " = " << nIteration << endl;

Info<< "Reading field T" << endl;
volScalarField T
(
    IOobject
    (
        "T",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info << "Reading field gradT" << endl;
surfaceVectorField gradT
(
    "gradT",
    fvc::interpolate(fvc::grad(T))
);

Info<< "Reading field uniT" << endl;
volScalarField uniT
(
    IOobject
    (
        "uniT",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "Reading field phi" << endl;
surfaceScalarField phi
(
    IOobject
    (
        "phi",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);
phi.oldTime();

Info<< "Reading field U" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    fvc::reconstruct(phi)
);

Info<< "Reading field Uf" << endl;
surfaceVectorField Uf
(
    IOobject
    (
        "Uf",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    linearInterpolate(U)
);
Uf += (phi - (Uf & mesh.Sf()))*mesh.Sf()/sqr(mesh.magSf());
Uf.write();

Info << "Reading field meshPhi" << endl;
surfaceScalarField meshPhi
(
    IOobject
    (
        "meshPhi",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    fvc::meshPhi(U)
);
meshPhi.oldTime();

// Colin parameter
Info << "Reading field A" << endl;
volScalarField A
(
    IOobject("A", runTime.timeName(), mesh,
             IOobject::MUST_READ, IOobject::AUTO_WRITE),
    mesh
);
