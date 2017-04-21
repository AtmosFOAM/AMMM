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

surfaceVectorField Uf
(
    IOobject("Uf", runTime.timeName(), mesh),
    fvc::interpolate(U)
);
Uf.oldTime();
Uf.write();

surfaceScalarField hf("hf", fvc::interpolate(h));

h *= 0;
const volScalarField h0("h0", 0*h + dimensionedScalar("h0", dimLength, 1));
hf = fvc::interpolate(h0, "interpolate(h)");

surfaceVectorField hUf
(
    IOobject("hUf", runTime.timeName(), mesh),
    hf*Uf
);

volVectorField hU
(
    IOobject("hU", runTime.timeName(), mesh),
    h0*U
);

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

volScalarField del2h
(
    IOobject
    (
        "del2h", runTime.timeName(), mesh,
        IOobject::NO_READ, IOobject::AUTO_WRITE
    ),
    fvc::laplacian(h)
);
del2h.write();

volScalarField del4h
(
    IOobject
    (
        "del4h", runTime.timeName(), mesh,
        IOobject::NO_READ, IOobject::AUTO_WRITE
    ),
    fvc::laplacian(del2h, "laplacian(h)")
);
del4h.write();

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
    ACblend = calcACblend(del4h);
}
ACblend.write();

