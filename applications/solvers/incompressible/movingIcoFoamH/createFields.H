HodgeOps H(rMesh);

// Uf is the velocity on the faces of the moving mesh
Info<< "Reading field Uf" << endl;
surfaceVectorField Uf
(
    IOobject
    (
        "Uf", runTime.timeName(), rMesh,
        IOobject::MUST_READ, IOobject::AUTO_WRITE
    ),
    rMesh
);

Info<< "Reading pressure, p" << endl;
volScalarField p
(
    IOobject
    (
        "p", runTime.timeName(), rMesh,
        IOobject::MUST_READ, IOobject::AUTO_WRITE
    ),
    rMesh
);

// Uf with component in direction H.delta corrected
surfaceVectorField Ufv("Ufv", Uf);

// Volumes associated with faces
surfaceScalarField faceVol("faceVol", 1/3.*(H.delta() & rMesh.Sf()));
faceVol.oldTime();

// fixed flux boundary conditions
wordList VuBCs(rMesh.boundaryMesh().size(), "calculated");
wordList uBCs(rMesh.boundaryMesh().size(), "zeroGradient");
forAll(rMesh.boundaryMesh(), patchi)
{
    const word btype = rMesh.boundaryMesh()[patchi].type();
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

Info << "Calculating the cell centre velocity, u" << endl;
volVectorField u
(
    IOobject("u", runTime.timeName(), rMesh,
             IOobject::READ_IF_PRESENT,IOobject::AUTO_WRITE),
    H.reconstruct(Uf & rMesh.Sf()),
    uBCs
);
u.write();

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

Info<< "Initialising the absolute face flux, U" << endl;
surfaceScalarField U
(
    IOobject("U", runTime.timeName(), rMesh),
    Uf & rMesh.Sf()
);

Info << "Calculating V mass circulation field" << endl;
surfaceScalarField V
(
    IOobject
    (
        "V", runTime.timeName(), rMesh,
        IOobject::READ_IF_PRESENT, IOobject::AUTO_WRITE
    ),
    Uf & H.delta(),
    VuBCs
);

Info << "Initialising dVdt for trapezoidal implicit time-stepping" << endl;
surfaceScalarField dVdt
(
    "dVdt",
    -H.delta() & 
    (
        fvc::interpolate(fvc::div(U-meshPhi, u, "div(U,u)"))
      //+ 2*(Omega^(Uf - Ug))
    )
);
dVdt.oldTime();

Info << "Storing the old value of V, V0 on the new mesh" << endl;
surfaceScalarField V0("V", V);

// fix no flow boundaries
forAll(V.boundaryField(), patchi)
{
    const word btype = rMesh.boundaryMesh()[patchi].type();
    const word utype = Uf.boundaryField()[patchi].type();
    if
    (
        btype == "wall" || btype == "symmetryPlane" || btype == "empty"
    )
    {
        V.boundaryFieldRef()[patchi] == 0;
    }
    else if(utype == "fixedValue")
    {
        u.boundaryFieldRef()[patchi] == Uf.boundaryField()[patchi];
    }
}

// Vorticity (for the monitor function)
volScalarField vorticity
(
    IOobject
    (
        "vorticity", runTime.timeName(), rMesh,
        IOobject::NO_READ, IOobject::AUTO_WRITE
    ),
    fvc::curl(Uf) & meshNormal
);
