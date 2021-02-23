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

Info << "Reading mountain height, h0, if defined\n" << endl;
volScalarField h0
(
    IOobject
    (
        "h0",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    mesh,
    dimensionedScalar("h0", dimLength, scalar(0))
);
Info << "h0 goes from " << min(h0).value() << " to " << max(h0).value()
     << endl;
const Switch withMountain = max(h0).value() - min(h0).value() > SMALL;

Info << "Reading in sponge tensor, muSponge, if defined\n" << endl;
volTensorField muSponge
(
    IOobject
    (
        "muSponge",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    mesh,
    dimensionedTensor("muSponge", dimless, tensor::zero)
);
const bool includeSponge = (max(mag(muSponge)).value() > SMALL);
Info << "muSponge goes from " << min(mag(muSponge)) << " to "
     << max(mag(muSponge)) << endl;

Info << "Reading in diffusion coefficient, diffusionCoeff, if defined\n"
     << endl;
volScalarField diffusionCoeff
(
    IOobject
    (
        "diffusionCoeff",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    mesh,
    dimensionedScalar("diffusionCoeff", dimensionSet(0,2,-1,0,0), scalar(0))
);
const bool includeDiffusion = (max(mag(diffusionCoeff)).value() > SMALL);
Info << "diffusionCoeff goes from " << min(mag(diffusionCoeff)) << " to "
     << max(mag(diffusionCoeff)) << endl;

surfaceVectorField Uf
(
    IOobject("Uf", runTime.timeName(), mesh),
    fvc::interpolate(U)
);
Uf.oldTime();
Uf.write();

surfaceScalarField hf("hf", fvc::interpolate(h));
hf.oldTime();

surfaceVectorField hUf
(
    IOobject("hUf", runTime.timeName(), mesh),
    hf*Uf
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

