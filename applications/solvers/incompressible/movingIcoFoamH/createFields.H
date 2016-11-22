// Read in prognostic variables
Info<< "Reading field Uf\n" << endl;
surfaceVectorField Uf
(
    IOobject
    (
        "Uf",
        runTime.timeName(),
        pMesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    pMesh
);

Info<< "Reading pressure, p\n" << endl;
volScalarField p
(
    IOobject
    (
        "p",
        runTime.timeName(),
        pMesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    pMesh
);

// fixed flux boundary conditions
wordList VuBCs(pMesh.boundaryMesh().size(), "calculated");
wordList uBCs(pMesh.boundaryMesh().size(), "zeroGradient");
forAll(pMesh.boundaryMesh(), patchi)
{
    const word btype = pMesh.boundaryMesh()[patchi].type();
    const word utype = Uf.boundaryField()[patchi].type();
    if (btype == "wall" || btype == "symmetryPlane" || btype == "empty")
    {
        VuBCs[patchi] = "fixedValue";
        uBCs[patchi] = "slip";
    }
    else if (utype == "fixedValue")
    {
        VuBCs[patchi] = "fixedValue";
        uBCs[patchi] = "fixedValue";
    }
}

// Cell centre velocity
volVectorField u
(
 IOobject("u", runTime.timeName(), pMesh,IOobject::READ_IF_PRESENT,IOobject::AUTO_WRITE),
    H.reconstruct(Uf & pMesh.Sf()),
    uBCs
);
u.write();

volVectorField q // the vorticity
(
 IOobject("q", runTime.timeName(), pMesh,IOobject::READ_IF_PRESENT,IOobject::AUTO_WRITE),
 fvc::curl(u)
);
q.write();


Info<< "Calculating face flux field U\n" << endl;
surfaceScalarField U
(
    IOobject
    (
       "U",
        runTime.timeName(),
        pMesh
    ),
    Uf & pMesh.Sf()
);
surfaceScalarField meshPhi = U;

surfaceScalarField Ucor
(
    IOobject
    (
       "Ucor",
        runTime.timeName(),
       pMesh,
       IOobject::READ_IF_PRESENT,
       IOobject::AUTO_WRITE
    ),
    U
);

Info << "Calculating V mass circulation field\n" << endl;
surfaceScalarField V
(
    IOobject("V", runTime.timeName(), pMesh),
    Uf & H.delta(),
    VuBCs
);
V.oldTime();

surfaceScalarField V0 = V;

// Rates of change for Crank-Nicholson
surfaceScalarField dVdt
(
    IOobject("dVdt", runTime.timeName(), pMesh),
    -(H.delta() & (fvc::interpolate(fvc::div(U,u))/*+2*(Omega^(Uf-Ug))*/))
    - H.magd()*fvc::snGrad(p),
     VuBCs
);
dVdt.oldTime();

// fix no flow boundaries
forAll(dVdt.boundaryField(), patchi)
{
    const word btype = pMesh.boundaryMesh()[patchi].type();
    const word utype = Uf.boundaryField()[patchi].type();
    if
    (
        btype == "wall" || btype == "symmetryPlane" || btype == "empty"
    )
    {
        V.boundaryFieldRef()[patchi] == 0;
        dVdt.boundaryFieldRef()[patchi] == 0;
    }
    else if(utype == "fixedValue")
    {
        u.boundaryFieldRef()[patchi] == Uf.boundaryField()[patchi];
    }
}

