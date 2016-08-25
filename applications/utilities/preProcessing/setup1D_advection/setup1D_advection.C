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
    setup1D_advection

Description
    Set the initial U and h for a shallow water eqn jet on a beta plane

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "mathematicalConstants.H"


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
    const dimensionedScalar T0(initDict.lookup("T0"));
    const scalar orog_height(readScalar(initDict.lookup("orog_height")));
    const vector wave_centre(initDict.lookup("wave_centre"));
    
    
   
    Info<< "Reading environmental properties\n" << endl;

    IOdictionary envDict
    (
        IOobject
        (
            "environmentalProperties",
            mesh.time().constant(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    );

    // gravity magnitude
    const dimensionedScalar g(envDict.lookup("g"));
    // beta
    const dimensionedScalar beta(envDict.lookup("beta"));


    // Create orography
    volScalarField h0
    (
        IOobject("h0", runTime.timeName(), mesh),
        mesh,
        dimensionedScalar("h0",dimLength,scalar(0)),
        "zeroGradient"
    );

    
    // Create initial height and velocity field
    volScalarField T
    (
        IOobject("T", runTime.timeName(), rMesh),
        rMesh,
        T0,
        "zeroGradient"
    );

    
    volVectorField U
    (
        IOobject("U", runTime.timeName(), rMesh),
        rMesh,
        dimensionedVector("U", dimLength/dimTime, Foam::vector(1,0,0)*u0.value())
    );
        
    surfaceVectorField Uf
    (
        IOobject("Uf", runTime.timeName(), rMesh),
        fvc::interpolate(U)
    );

    const volVectorField& C = rMesh.C();

    scalar xlength=rMesh.bounds().span().x();
    Info<< "xlength = " << xlength << endl;

    //set the total wave height and velocity
    forAll(T, i)
        {
            scalar dist = mag((rMesh.C()[i]/xlength) - wave_centre);
            T[i] = T[i] + 4.94134215e+02/sqr(Foam::cosh(1.98087675e+01*dist));
            //U[i] = U[i] + Foam::vector(1,0,0)*15.30774496/sqr(Foam::cosh(19.53326431*dist));
        }


    //set the orography
    forAll(h0, i)
    {
        const vector& Ci = C[i];
        if (Ci.x() > 20000000 and Ci.x() < 24000000) {
            h0[i] = orog_height;
        }
     }


    
    //h = hTotal-h0;
    
    //h.write();
    //h0.write();
    T.write();
    U.write();
    Uf.write();
    //volVectorField hU
    //(
    //     IOobject("hU", runTime.timeName(), rMesh),
    //     h*U,
    //     "slip"
    // );
    //hU.write();

    Info<< "End\n" << endl;

    return(0);
}

// ************************************************************************* //
