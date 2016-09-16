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

Info<< "Reading field t\n" << endl;

// volScalarField h
// (
//  IOobject
//  (
//   "h",
//   runTime.timeName(),
//   mesh,
//   IOobject::MUST_READ,
//   IOobject::AUTO_WRITE
//   ),
//  mesh
//  );


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

//pointVectorField meshUpoints = fvc::faceToPointReconstructFake(
//                                                           ((fvc::interpolate(rMeshU)       //interpolate to faces
//                                                             & rMesh.Sf())/rMesh.magSf())     // get surface normal component
//                                                           );
//pointField thingy = pMesh.points();
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

