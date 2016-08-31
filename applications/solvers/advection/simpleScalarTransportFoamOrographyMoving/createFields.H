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

Info<< "Reading field t\n" << endl;

volScalarField t
(
 IOobject
 (
  "t",
  runTime.timeName(),
  rMesh,
  IOobject::MUST_READ,
  IOobject::AUTO_WRITE
  ),
 rMesh
 );

Info<< "Reading field ht\n" << endl;

volScalarField h
(
 IOobject
 (
  "h",
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

surfaceScalarField phi
(
 IOobject
 (
  "phi",
  runTime.timeName(),
  rMesh,
  IOobject::READ_IF_PRESENT,
  IOobject::AUTO_WRITE
  ),
 linearInterpolate(U) & rMesh.Sf()
 );

//The mesh velocity
volVectorField rMeshU
(
 IOobject
 (
  "rMeshU",
  runTime.timeName(),
  rMesh,
  IOobject::MUST_READ,
  IOobject::AUTO_WRITE
  ),
 rMesh
 );


pointVectorField meshUpoints = fvc::faceToPointReconstruct(
                                                           ((fvc::interpolate(rMeshU)       //interpolate to faces
                                                             & rMesh.Sf())/rMesh.magSf())     // get surface normal component
                                                           );


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
