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
    setPlaneJet

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
    // y location of the centre of the jet
    const dimensionedScalar yc(initDict.lookup("jetCentre"));
    // jet half width
    const dimensionedScalar w(initDict.lookup("jetHalfWidth"));
    // Shallow water height at the equator
    const dimensionedScalar he(initDict.lookup("he"));
    
    // Height, location and widths of a bump
    const dimensionedScalar bumpHeight
    (
        initDict.lookupOrDefault<dimensionedScalar>
        (
            "bumpHeight", dimensionedScalar("bumpHeight", dimLength, scalar(0))
        )
    );
    const dimensionedVector bumpCentre
    (
        initDict.lookupOrDefault<dimensionedVector>
        (
            "bumpCentre", dimensionedVector("bumpCentre", dimLength, point::zero)
        )
    );
    const dimensionedVector bumpWidth
    (
        initDict.lookupOrDefault<dimensionedVector>
        (
            "bumpWidth", dimensionedVector("bumpWidth", dimLength, point::zero)
        )
    );
    
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
    volScalarField hTotal
    (
        IOobject("hTotal", runTime.timeName(), mesh),
        mesh,
        he,
        "zeroGradient"
    );

    
    // Create initial height and velocity field
    volScalarField h
    (
        IOobject("h", runTime.timeName(), mesh),
        mesh,
        he,
        "zeroGradient"
    );

    
    volVectorField U
    (
        IOobject("U", runTime.timeName(), mesh),
        mesh,
        dimensionedVector("U", dimLength/dimTime, Foam::vector(1,0,0)*20)
    );
        
    surfaceVectorField Uf
    (
        IOobject("Uf", runTime.timeName(), mesh),
        fvc::interpolate(U)
    );

    const volVectorField& C = mesh.C();
    forAll(h, i)
    {
        const vector& Ci = C[i];
        if (Ci.x() < 1000000) {
            hTotal[i] *= 1.3;
        }
        Info<< Ci << h[i] << endl;
        if (Ci.x() > 20000000 and Ci.x() < 24000000) {
            h0[i] = 7500;
        }
        Info<< Ci << h0[i] << endl;
     }
    h = hTotal-h0;
    
    h.write();
    h0.write();
    hTotal.write();
    U.write();
    Uf.write();
    volVectorField hU
    (
        IOobject("hU", runTime.timeName(), mesh),
        h*U,
        "slip"
    );
    hU.write();

    Info<< "End\n" << endl;

    return(0);
}

// ************************************************************************* //
