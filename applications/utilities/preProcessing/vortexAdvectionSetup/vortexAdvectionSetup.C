/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
  \\    /   O peration     |
  \\  /    A nd           | Copyright (C) 1991-2007 OpenCFD Ltd.
  \\/     M anipulation  |
  -------------------------------------------------------------------------------
  License
  This file is part of OpenFOAM.

  OpenFOAM is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License
  along with OpenFOAM; if not, write to the Free Software Foundation,
  Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

  Application
  ringSetFlow

  Description
  Set the initial U and h for a shallow water eqn jet on a beta plane

  \*---------------------------------------------------------------------------*/
#include "HodgeOps.H"
#include "fvCFD.H"
#include "mathematicalConstants.H"
#include "setInternalValues.H"
#include "ringMesh.H"
#include "StreamFunctionVortexAdvection.H"
using namespace Foam::constant::mathematical;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{

#   include "setRootCase.H"
#   include "createTime.H"
#   include "createMesh.H"

    // Create the physical mesh
    fvMesh pMesh
        (
         Foam::IOobject
         (
          "pMesh", runTime.timeName(), runTime,
          IOobject::MUST_READ, IOobject::AUTO_WRITE
          )
         );
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    HodgeOps H(pMesh);
#define dt runTime.deltaT()
    
    scalar lx = pMesh.bounds().span().x();
    scalar ly = pMesh.bounds().span().y();
    scalar lz = pMesh.bounds().span().z();

    Info << "lx = " << lx << " ly = " << ly << " lz = " << lz << endl;

    Info<< "Reading initial conditions\n" << endl;

    IOdictionary initDict
        (
         IOobject
         (
          "initialConditions",
          mesh.time().constant(),
          mesh,
          IOobject::MUST_READ,
          IOobject::NO_WRITE
          )
         );


    const vector vortexCentre(initDict.lookup("vortexCentre"));
    const scalar vortexRadius(readScalar(initDict.lookup("vortexRadius")));
    const scalar vortexMagnitude(readScalar(initDict.lookup("vortexMagnitude")));
    const dimensionedVector constantAdvection(initDict.lookup("constantAdvection"));
    
    Info<< "Reading environmental properties\n" << endl;


    Info<< "Reading field U\n" << endl;

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
    Info<< "Read field Uf\n" << endl;


    // the following is from movingIcoFoamH createFields.H to set up
    // the appropriate pressure field
    
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
    //    u.write();
    volVectorField q // the vorticity
        (
         IOobject("q", runTime.timeName(), pMesh,IOobject::READ_IF_PRESENT,IOobject::AUTO_WRITE),
         fvc::curl(u)
         );


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

    Uf *= 0.0;
 
    forAll(Uf,face){
        scalar x = (Uf.mesh().faceCentres()[face].x()-vortexCentre.x());
        scalar y = (Uf.mesh().faceCentres()[face].y()-vortexCentre.y());
        scalar z = (Uf.mesh().faceCentres()[face].z()-vortexCentre.z());
        
        vector direction = vector(z,0,-x);
        direction /=mag(direction);
        scalar vortexDistance = mag(vector(x,y,z));
        Uf[face] = direction * vortexMagnitude*Foam::exp(-sqr(vortexDistance)/(2*sqr(vortexRadius)));//*vortexDistance);//* Foam::exp(-(  sqr(vortexDistance-vortexRadius) ));

    }

    forAll(Uf,face){
        scalar x = (Uf.mesh().faceCentres()[face].x()-vortexCentre.x());
        scalar y = (Uf.mesh().faceCentres()[face].y()-vortexCentre.y());
        scalar z = (Uf.mesh().faceCentres()[face].z()-(vortexCentre.z()+3*vortexRadius));
        
        vector direction = vector(z,0,-x);
        direction /=mag(direction);
        scalar vortexDistance = mag(vector(x,y,z));
        Uf[face] -= direction * vortexMagnitude*Foam::exp(-sqr(vortexDistance)/(2*sqr(vortexRadius)));//*vortexDistance);//* Foam::exp(-(  sqr(vortexDistance-vortexRadius) ));

    }

    
    //Uf += constantAdvection;

    const scalar dx = mesh.bounds().span().x();
    const dimensionedScalar u0(initDict.lookup("u0"));
    const scalar orog_height(readScalar(initDict.lookup("orog_height")));
    const scalar orog_start(readScalar(initDict.lookup("orog_start")));
    const scalar orog_stop(readScalar(initDict.lookup("orog_stop")));
    Info << "Calling StokesTheoremU to calculate initial velocities" << endl;
    //    #include "StokesTheoremU.H"

    //U.write();
    //Uf = U*(pMesh.Sf()/sqr(pMesh.magSf()));
    //u = fvc::reconstruct(U);
    Uf.write();







    u *= 0.0;
 
    forAll(u,celli){
        scalar x = (u.mesh().C()[celli].x()-vortexCentre.x());
        scalar y = (u.mesh().C()[celli].y()-vortexCentre.y());
        scalar z = (u.mesh().C()[celli].z()-vortexCentre.z());
        
        vector direction = vector(z,0,-x);
        direction /=mag(direction);
        scalar vortexDistance = mag(vector(x,y,z));
        u[celli] = direction * vortexMagnitude*Foam::exp(-sqr(vortexDistance)/(2*sqr(vortexRadius)));//*vortexDistance);//* Foam::exp(-(  sqr(vortexDistance-vortexRadius) ));
    }

    forAll(u,celli){
        scalar x = (u.mesh().C()[celli].x()-vortexCentre.x());
        scalar y = (u.mesh().C()[celli].y()-vortexCentre.y());
        scalar z = (u.mesh().C()[celli].z()-(vortexCentre.z()+3*vortexRadius));
        
        vector direction = vector(z,0,-x);
        direction /=mag(direction);
        scalar vortexDistance = mag(vector(x,y,z));
        u[celli] -= direction * vortexMagnitude*Foam::exp(-sqr(vortexDistance)/(2*sqr(vortexRadius)));//*vortexDistance);//* Foam::exp(-(  sqr(vortexDistance-vortexRadius) ));
    }

    
    u.write();
    q = fvc::curl(u);
    q.write();
    p.write();


    Info<< "End\n" << endl;
    
    return(0);
}

// ************************************************************************* //
