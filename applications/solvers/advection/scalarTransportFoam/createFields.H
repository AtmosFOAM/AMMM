Info<< "Reading field T\n" << endl;

volScalarField T
(
    IOobject
    (
        "T",
        runTime.timeName(),
        rMesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    rMesh
);


Info<< "Reading field phi\n" << endl;

surfaceScalarField phi
(
    IOobject
    (
        "phi", runTime.timeName(), rMesh,
        IOobject::MUST_READ, IOobject::AUTO_WRITE
    ),
    rMesh
);

// Read U if present, otherwise create and write (not used)
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        rMesh,
        IOobject::READ_IF_PRESENT,
        IOobject::NO_WRITE
    ),
    fvc::reconstruct(phi)
);
U.write();

// Read Uf if present, otherwise create and write (not used)
surfaceVectorField Uf
(
    IOobject
    (
        "Uf",
        runTime.timeName(),
        rMesh,
        IOobject::READ_IF_PRESENT,
        IOobject::NO_WRITE
    ),
    linearInterpolate(fvc::reconstruct(phi))
);
Uf += (phi - (Uf & rMesh.Sf()))*rMesh.Sf()/sqr(rMesh.magSf());
Uf.write();

// Store div(phi,T) for time-stepping
volScalarField divPhiT = fvc::div(phi, T);
divPhiT.oldTime();

// Calculate divU for checking divergence free
volScalarField divU("divU", fvc::div(phi));
divU.write();

Info << "Calculating the rMesh flux, meshPhi" << endl;
surfaceScalarField meshPhi
(
    IOobject
    (
        "meshPhi", runTime.timeName(), rMesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    fvc::meshPhi(U)
);

// Gradient of T for the monitor function
surfaceVectorField gradT
(
    IOobject
    (
        "gradT", runTime.timeName(), rMesh,
        IOobject::NO_READ, IOobject::AUTO_WRITE
    ),
    fvc::interpolate(fvc::grad(T, "gradT"), "gradT")
);

