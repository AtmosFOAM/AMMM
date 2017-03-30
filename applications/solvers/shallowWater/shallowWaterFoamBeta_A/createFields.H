Info<< "Reading field u, velocity on cell centres" << endl;
volVectorField u
(
    IOobject
    (
        "u", runTime.timeName(), rMesh,
        IOobject::MUST_READ, IOobject::AUTO_WRITE
    ),
    rMesh
);

Info<< "Reading field h" << endl;
volScalarField h
(
    IOobject
    (
        "h", runTime.timeName(), rMesh,
        IOobject::MUST_READ, IOobject::AUTO_WRITE
    ),
    rMesh
);
h.correctBoundaryConditions();

// h interpolated on to faces
surfaceScalarField hf
(
    IOobject("hf", runTime.timeName(), rMesh),
    fvc::interpolate(h)
);

// momentum vector
volVectorField hu("hu", h*u);
hu.oldTime();

Info<< "Reading field h0 if present (the orography) on the fixed mesh\n" << endl;
volScalarField h0
(
    IOobject
    (
        "h0", runTime.timeName(), mesh, IOobject::READ_IF_PRESENT
    ),
    mesh,
    dimensionedScalar("h0", dimLength, 0.0)
);
// The gradient of the orography is assumed not to change
//const surfaceScalarField gsnGradh0 = mesh.magSf()*g*fvc::snGrad(h0);


// fixed flux boundary conditions
wordList VuBCs(rMesh.boundaryMesh().size(), "calculated");
forAll(rMesh.boundaryMesh(), patchi)
{
    const word btype = rMesh.boundaryMesh()[patchi].type();
    const word utype = u.boundaryField()[patchi].type();
    const word htype = h.boundaryField()[patchi].type();
    if (btype == "wall" || btype == "symmetryPlane" || btype == "empty"
     || htype == "geostrophicZeroFlux")
    {
        VuBCs[patchi] = "fixedValue";
    }
    else if (utype == "fixedValue")
    {
        VuBCs[patchi] = "fixedValue";
    }
}

Info << "Calculating the mesh flux, meshPhi" << endl;
surfaceScalarField meshPhi
(
    IOobject
    (
        "meshPhi", runTime.timeName(), rMesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    fvc::meshPhi(u)
);

// The change in volume per time-step
volScalarField volRatio
(
    IOobject("volRatio", runTime.timeName(), rMesh),
    rMesh,
    dimensionedScalar("volRatio", dimless, scalar(1))
);

Info<< "Initialising the absolute face flux, U" << endl;
surfaceScalarField U
(
    IOobject
    (
        "U", runTime.timeName(), rMesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    hf * (linearInterpolate(u) & rMesh.Sf())
);
U.write();

Info << "Calculating V mass circulation field" << endl;
surfaceScalarField V
(
    IOobject
    (
        "V", runTime.timeName(), rMesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    hf * (linearInterpolate(u) & H.delta()),
    VuBCs
);

//Info<< "Creating field hTotal for post processing\n" << endl;
//volScalarField hTotal
//(
//   IOobject
//   (
//       "hTotal", runTime.timeName(), mesh,
//       IOobject::NO_READ_IF_PRESENT, IOobject::AUTO_WRITE
//   ),
//   h+h0
//);
//hTotal.write();

// Flux divergence at old time-step for time-stepping
volScalarField divUold("divUold", fvc::div(U));

// fix no flow boundaries
forAll(V.boundaryField(), patchi)
{
    const word btype = rMesh.boundaryMesh()[patchi].type();
    const word utype = u.boundaryField()[patchi].type();
    const word htype = h.boundaryField()[patchi].type();
    if
    (
        btype == "wall" || btype == "symmetryPlane" || btype == "empty"
     || utype == "slip" || htype == "geostrophicZeroFlux"
    )
    {
        V.boundaryFieldRef()[patchi] == 0;
    }
}

