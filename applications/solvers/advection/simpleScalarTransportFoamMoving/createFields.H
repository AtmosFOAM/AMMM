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


    Info<< "Reading field U\n" << endl;

    const volVectorField U
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
