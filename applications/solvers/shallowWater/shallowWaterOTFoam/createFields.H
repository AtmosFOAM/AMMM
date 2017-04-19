Info<< "Reading field h\n" << endl;
volScalarField h
(
    IOobject
    (
        "h",
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
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

surfaceVectorField hUf
(
    IOobject("hUf", runTime.timeName(), mesh),
    fvc::interpolate(h*U)
);

surfaceScalarField hf("hf", fvc::interpolate(h));

surfaceScalarField phi
(
    IOobject
    (
        "phi", runTime.timeName(), mesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    hf*(fvc::interpolate(U) & mesh.Sf())
);

volScalarField divPhi
(
    IOobject
    (
        "divPhi", runTime.timeName(), mesh,
        IOobject::NO_READ, IOobject::AUTO_WRITE
    ),
    fvc::div(phi)
);
divPhi.write();

volScalarField ACblend
(
    IOobject
    (
        "ACblend", runTime.timeName(), mesh,
        IOobject::NO_READ, IOobject::AUTO_WRITE
    ),
    mesh,
    dimensionedScalar("b", dimless, ACblendConst)
);
if (ACblendConst < -SMALL)
{
    ACblend = calcACblend(divPhi);
}
ACblend.write();

