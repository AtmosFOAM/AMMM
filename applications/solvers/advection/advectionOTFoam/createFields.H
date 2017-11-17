const label nIteration
(
    readLabel(mesh.solutionDict().lookup("nIteration"))
);
Info<< "nIteraion = " << nIteration << endl;

Info<< "Reading field T\n" << endl;
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

surfaceVectorField gradT
(
    "gradT",
    fvc::interpolate(fvc::grad(T))
);

Info<< "Reading field uniT\n" << endl;
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

Info<< "Reading field phi\n" << endl;
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

Info<< "Reading field U\n" << endl;
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

Info<< "Reading field Uf\n" << endl;
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

Info << "Reading field meshPhi\n" << endl;
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

volScalarField divU("divU", fvc::div(phi));
divU.write();

// Colin parameter
volScalarField A
(
    IOobject("A", runTime.timeName(), mesh,
             IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE),
    mesh,
    dimensionedScalar("A", dimless, scalar(1))
);

volScalarField AT
(
    IOobject("AT", runTime.timeName(), mesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    A*T
);
AT.write();

volScalarField AuniT
(
    IOobject("AuniT", runTime.timeName(), mesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    A*uniT
);
AuniT.write();

volScalarField flowOverGround
(
    IOobject("flowOverGround", runTime.timeName(), mesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    mesh,
    dimensionedScalar("flowOverGround", dimensionSet(0,3,-1,0,0), scalar(0))
);

