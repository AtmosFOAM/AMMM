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

// h interpolated on to faces
surfaceScalarField hf
(
    IOobject("hf", runTime.timeName(), mesh),
    fvc::interpolate(h)
);

// h0 is defined to be the orography
Info<< "Reading field h0 if present (the orography)\n" << endl;
volScalarField h0
(
    IOobject
    (
        "h0",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT
    ),
    mesh//,
    //dimensionedScalar("h0", dimLength, 0.0)
);
Info<< "Read h0: max = " << max(h0) << endl;

// The gradient of the orography is assumed not to change
const surfaceScalarField gsnGradh0 = mesh.magSf()*g*fvc::snGrad(h0);


// The cell centre velocity
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

Info<< "calculating field Uf - the velocity on the face\n" << endl;
surfaceVectorField Uf
(
    IOobject
    (
        "Uf",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    fvc::interpolate(U)
    //    dimensionedVector("Uf", dimVelocity, vector(0 0 0))
);


Info<< "Reading/calculating face flux field\n" << endl;

surfaceScalarField flux
(
    IOobject
    (
        "flux",
        runTime.timeName(),
        mesh,
        IOobject::READ_IF_PRESENT,
        IOobject::AUTO_WRITE
    ),
    hf*(Uf & mesh.Sf())
);
flux.oldTime();


Info<< "Creating field hTotal for post processing\n" << endl;
volScalarField hTotal
(
   IOobject
   (
       "hTotal",
       runTime.timeName(),
       mesh,
       IOobject::READ_IF_PRESENT,
       IOobject::AUTO_WRITE
   ),
   h+h0
);
hTotal.write();

Info<< "Creating Coriolis Force on the face, f (Sf x k)" << endl;
const surfaceVectorField fSfxk
(
    "fSfxk",
    beta*mesh.Cf().component(vector::Y)*(mesh.Sf() ^ vector(0.,0.,1.))
);

// Rate of change of flux
surfaceScalarField dFluxdt = -hf*(gsnGradh0 + g*fvc::snGrad(h)*mesh.magSf())
                          - (fvc::interpolate(fvc::div(flux,U)) & mesh.Sf());
Info<< "before rotating in createfields\n" << endl;
if (rotating) dFluxdt -= hf*(fSfxk & Uf);
dFluxdt.oldTime();

// Courant numbers

surfaceScalarField CoNum = mesh.surfaceInterpolation::deltaCoeffs()*mag(flux)
                         / (fvc::interpolate(h)*mesh.magSf())*runTime.deltaT();

surfaceScalarField waveCoNum = scalar(0.5)
                               *mesh.surfaceInterpolation::deltaCoeffs()
                               *sqrt(mag(fvc::interpolate(h)))
                               *sqrt(g)*runTime.deltaT();

//fluxU with boundaries to ensure correct flow over boundaries
wordList fluxUBCs(mesh.boundaryMesh().size(), "calculated");
forAll(mesh.boundaryMesh(), patchi)
{
    const word btype = mesh.boundaryMesh()[patchi].type();
    const word htype = h.boundaryField()[patchi].type();
    if
    (
        btype == "wall" || btype == "symmetryPlane" || btype == "empty"
     || htype == "geostrophicZeroFlux" || htype == "zeroGradient"
    )
    {
        fluxUBCs[patchi] = "fixedValue";
    }
}

surfaceScalarField fluxU
(
    IOobject("fluxU", runTime.timeName(), mesh),
    flux,
    fluxUBCs
);

forAll(fluxU.boundaryField(), patchi)
{
    const word btype = mesh.boundaryMesh()[patchi].type();
    const word htype = h.boundaryField()[patchi].type();
    if
    (
        btype == "wall" || btype == "symmetryPlane" || btype == "empty"
     || htype == "geostrophicZeroFlux"
    )
    {
        static_cast<Foam::fvsPatchField<double>>(fluxU.boundaryField()[patchi]) == 0;
    }
}


//#include "faceToPointReconstruct.H"
// The mesh velocity
volVectorField meshU
(
    IOobject
    (
        "meshU",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);


pointVectorField meshUpoints = fvc::faceToPointReconstruct(
              ((fvc::interpolate(meshU)       //interpolate to faces
                & mesh.Sf())/mesh.magSf())     // get surface normal component
                                               );
