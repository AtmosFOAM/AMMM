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
volScalarField Told("Told", T);

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
        IOobject::AUTO_WRITE
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

// The change in volume per time-step
volScalarField volRatio
(
    IOobject("volRatio", runTime.timeName(), rMesh),
    rMesh,
    dimensionedScalar("volRatio", dimless, scalar(1))
);

// Also advect a uniform tracer
volScalarField uniT
(
    IOobject("uniT", runTime.timeName(), rMesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    rMesh,
    dimensionedScalar("uniT", dimless, scalar(0.5)),
    T.boundaryField().types()
);
volScalarField uniTold("uniTold", uniT);
