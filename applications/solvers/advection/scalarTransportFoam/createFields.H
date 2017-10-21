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

Info<< "Reading field phi\n" << endl;
surfaceScalarField phi
(
    IOobject
    (
        "phi", runTime.timeName(), pMesh,
        IOobject::MUST_READ, IOobject::AUTO_WRITE
    ),
    pMesh
);
phi.oldTime();

// Read U if present, otherwise create and write (not used)
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        pMesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    fvc::reconstruct(phi)
);
U.oldTime();
U.write();

// Read Uf if present, otherwise create and write (not used)
surfaceVectorField Uf
(
    IOobject
    (
        "Uf",
        runTime.timeName(),
        pMesh,
        IOobject::READ_IF_PRESENT,
        IOobject::NO_WRITE
    ),
    linearInterpolate(fvc::reconstruct(phi))
);
Uf += (phi - (Uf & pMesh.Sf()))*pMesh.Sf()/sqr(pMesh.magSf());
Uf.write();

Info << "Calculating the pMesh flux, meshPhi" << endl;
surfaceScalarField meshPhi
(
    IOobject
    (
        "meshPhi", runTime.timeName(), pMesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    fvc::meshPhi(U)
);

/*// The change in volume per time-step
volScalarField volRatio
(
    IOobject("volRatio", runTime.timeName(), pMesh),
    pMesh,
    dimensionedScalar("volRatio", dimless, scalar(1))
);
*/
// Also advect a uniform tracer
volScalarField uniT
(
    IOobject("uniT", runTime.timeName(), pMesh,
             IOobject::MUST_READ, IOobject::AUTO_WRITE),
    pMesh
);

// Fluxes on the rMesh
surfaceScalarField phiR
(
    IOobject
    (
        "phiR", runTime.timeName(), rMesh, IOobject::MUST_READ
    ),
    rMesh
);

// Colin parameter
volScalarField A
(
    IOobject("A", runTime.timeName(), pMesh,
             IOobject::NO_READ, IOobject::AUTO_WRITE),
    pMesh,
    dimensionedScalar("A", dimless, scalar(1))
);

