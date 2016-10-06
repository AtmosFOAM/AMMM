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
  linearSetFlow

  Description
  Set the initial U and h for a shallow water eqn jet on a beta plane

  \*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "mathematicalConstants.H"
#include "setInternalValues.H"
#include "ringMesh.H"
#include "StreamFunctionAt.H"
using namespace Foam::constant::mathematical;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{

#   include "setRootCase.H"
#   include "createTime.H"
#   include "createMesh.H"

    // Create the moving mesh
    fvMesh rMesh
        (
         Foam::IOobject
         (
          "rMesh", runTime.timeName(), runTime,
          IOobject::MUST_READ, IOobject::AUTO_WRITE
          )
         );
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

    // Maximum jet velocity
    const dimensionedScalar u0(initDict.lookup("u0"));
    // Shallow water height at the equator
    const dimensionedScalar he(initDict.lookup("he"));
    const dimensionedScalar T0(initDict.lookup("T0"));
    const scalar orog_height(readScalar(initDict.lookup("orog_height")));
    const scalar orog_start(readScalar(initDict.lookup("orog_start")));
    const scalar orog_stop(readScalar(initDict.lookup("orog_stop")));

    Info<< "Reading environmental properties\n" << endl;


    Info<< "Reading field U\n" << endl;

    volVectorField U
        (
         IOobject
         (
          "U",
          runTime.timeName(),
          pMesh,
          IOobject::MUST_READ,
          IOobject::AUTO_WRITE
          ),
         pMesh
         );
    Info<< "Read field U\n" << endl;
    surfaceScalarField phi
        (
         IOobject
         (
          "phi",
          runTime.timeName(),
          pMesh,
          IOobject::READ_IF_PRESENT,
          IOobject::AUTO_WRITE
          ),
         linearInterpolate(U) & pMesh.Sf()
         );
    Info<< "Read field phi\n" << endl;


    Info<< "Reading field rh0 if present (the orography on the moving mesh)\n" << endl;
    volScalarField rh0
        (
         IOobject
         (
          "rh0",
          runTime.timeName(),
          rMesh,
          IOobject::READ_IF_PRESENT,
          IOobject::AUTO_WRITE
          ),
         rMesh//,
         //dimensionedScalar("h0", dimLength, 0.0)
         );


    surfaceScalarField rh0Faces
        (
         IOobject
         (
          "rh0Faces",
          runTime.timeName(),
          rMesh,
          IOobject::NO_READ,
          IOobject::AUTO_WRITE
          ),
         fvc::interpolate(rh0)
         //dimensionedScalar("h0", dimLength, 0.0)
         );


    Info << "Calculating phi from u0 and h0" << endl;
#include "StokesTheoremPhi.H"
    Info << "Reconstructing initial U from phi " << endl;
    U = fvc::reconstruct(phi);
    
    phi.write();
    U.write();
    


    Info<< "End\n" << endl;
    
    return(0);
}

// ************************************************************************* //
