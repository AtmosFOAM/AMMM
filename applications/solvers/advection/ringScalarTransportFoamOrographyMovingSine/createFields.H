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

volScalarField V
(
    IOobject
    (
        "V",
        runTime.timeName(),
        pMesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    pMesh
);
forAll(V,c){V[c] = pMesh.V()[c];}

volScalarField dV
(
    IOobject
    (
        "dV",
        runTime.timeName(),
        pMesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
        ),
    pMesh
);


//scalarField Mass = fvc::volumeIntegrate(T);
volScalarField Mass
(
    IOobject
    (
        "Mass",
        runTime.timeName(),
        pMesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
        ),
    pMesh
);
forAll(Mass,c) { Mass[c] = T.mesh().V()[c]*T[c]; }

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
Info<< "Read field U\n" << endl;
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
Info<< "Read field phi\n" << endl;
Info<< "Dimensions of pMesh.Sf() = " << pMesh.Sf().dimensions() << endl;
Info<< "Dimensions of phi = " << phi.dimensions() << endl;;
forAll(phi,p){phi[p] = 0.00001;}
phi[1] *= -1;


surfaceScalarField phiR
(
    IOobject
    (
        "phiR",
        runTime.timeName(),
        pMesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
        ),
    phi
);

surfaceScalarField phiT
(
    IOobject
    (
        "phiT",
        runTime.timeName(),
        pMesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
        ),
    fvc::interpolate(T)*phi
);

Info<< "Read field phiT\n" << endl;

pointField meshUpoints = pMesh.points();

Info<< "Reading field h0 if present (the orography)\n" << endl;
volScalarField h0
(
    IOobject
    (
        "h0",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::NO_WRITE
    ),
    mesh//,
    //dimensionedScalar("h0", dimLength, 0.0)
);

Info<< "Reading field rh0 if present (the orography on the moving mesh)\n" << endl;
volScalarField rh0
(
    IOobject
    (
        "rh0",
        runTime.timeName(),
        rMesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    rMesh//,
    //dimensionedScalar("h0", dimLength, 0.0)
);


surfaceScalarField rh0Faces
(
    IOobject
    (
        "rh0Faces",
        runTime.timeName(),
        rMesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    fvc::interpolate(rh0)
    //dimensionedScalar("h0", dimLength, 0.0)
);

