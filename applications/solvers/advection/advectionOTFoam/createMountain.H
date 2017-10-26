// Read in and create the mountain data

IOdictionary mountainDict
(
    IOobject
    (
        "mountainDict",
        mesh.time().system(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    )
);

autoPtr<terrainFollowingTransform> transform
(
    terrainFollowingTransform::New(mountainDict)
);

