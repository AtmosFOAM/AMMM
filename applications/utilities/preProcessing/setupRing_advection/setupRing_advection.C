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
    setupRing_advection

Description
    Set the initial U and h for a shallow water eqn jet on a beta plane

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "mathematicalConstants.H"
#include "setInternalValues.H"
#include "ringMesh.H"

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
    const vector wave_centre(initDict.lookup("wave_centre"));
    const scalar wave_angle(readScalar(initDict.lookup("wave_angle")));
    const scalar wave_height(readScalar(initDict.lookup("wave_height")));
    const scalar wave_width(readScalar(initDict.lookup("wave_width")));
   
    const scalar pi = Foam::constant::mathematical::pi;
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


    //Info << "The mesh has been read into setupRing_advection and volumes are: " << mesh.V();
    
    // Create orography
    volScalarField h0
    (
        IOobject("h0", runTime.timeName(), mesh),
        mesh,
        dimensionedScalar("h0",dimLength,scalar(0)),
        "zeroGradient"
    );


    // Create orography on the rMesh
    volScalarField rh0
    (
        IOobject("rh0", runTime.timeName(), rMesh),
        rMesh,
        dimensionedScalar("h0",dimLength,scalar(0)),
        "zeroGradient"
    );

    
    // Create initial height and velocity field
    volScalarField T
    (
        IOobject("T", runTime.timeName(), pMesh),
        pMesh,
        T0,
        "zeroGradient"
    );
    
    volScalarField V
    (
        IOobject("V", runTime.timeName(), pMesh),
        pMesh,
        T0,
        "zeroGradient"
    );

    volScalarField dV
        (
         IOobject("dV", runTime.timeName(), pMesh),
         pMesh,
         1.0,
         "zeroGradient"
         );


    
    volScalarField Mass
    (
        IOobject("Mass", runTime.timeName(), pMesh),
        pMesh,
        T0,
        "zeroGradient"
    );
    
    
    
    volVectorField U
    (
        IOobject("U", runTime.timeName(), pMesh),
        pMesh,
        dimensionedVector("U", dimLength/dimTime, Foam::vector(1,0,0)*u0.value())
    );
        
    surfaceVectorField Uf
    (
        IOobject("Uf", runTime.timeName(), pMesh),
        fvc::interpolate(U)
    );

    const volVectorField& C = rMesh.C();

    scalar xlength=rMesh.bounds().span().x();
    Info<< "xlength = " << xlength << endl;

    //set the total wave height and velocity
    forAll(T, i)
        {
            scalar theta = Foam::atan2(C[i].y(),C[i].x());
            scalar angle_difference = Foam::atan2( Foam::sin(theta-wave_angle), Foam::cos(theta-wave_angle)); 
            
            //scalar dist = mag((rMesh.C()[i]/xlength) - wave_centre);
            T[i] = T[i] + wave_height/sqr(Foam::cosh(angle_difference/wave_width));
            //U[i] = U[i] + Foam::vector(1,0,0)*15.30774496/sqr(Foam::cosh(19.53326431*dist));
        }


    //set the orography
    forAll(h0, i)
        {
            scalar theta = Foam::atan2(C[i].y(),C[i].x());
            if (theta > pi/2 and theta < 9*pi/16) {
                h0[i] = orog_height;
                //T[i] *= 1-orog_height;
            }
            //Info << "Theta = " << theta << " h0[i] = " << h0[i] << endl;
        }

    pointField targetPoints = pMesh.points();
    forAll(pMesh.points(),point){
        scalar x = pMesh.points()[point].x();
        scalar y = pMesh.points()[point].y();
        //scalar r = Foam::sqrt( sqr(x) + sqr(y) );
        //Info().precision(16);
        //Info << " r = " << r << endl;
        //if( r < 2 - 1.0e-5 ) { // i.e. not on the outer shell
        if( pMesh.points()[point].z() < 0.95 ) {
        scalar theta = Foam::atan2(y,x);
        if (theta > pi/2 and theta < 9*pi/16) {
            //set the new radius
            //scalar z_new = orog_height;
            targetPoints[point].z() = orog_height;
            //targetPoints[point].y() = r_new*Foam::sin(theta);
            //Info << "moving point " << point << "from r = " << r << " to " << r_new << endl;
        }
        }
    }
    pMesh.movePoints(targetPoints);
    pMesh.write();
    
    // Create height field
    volScalarField h
    (
        IOobject("h", runTime.timeName(), rMesh),
        rMesh,
        he,
        "zeroGradient"
    );

    forAll(h,cellI)
        {
            h[cellI] -= h0[cellI];
        }
    
    setInternalValues(rh0,h0);

    
    h.write();
    h0.write();
    rh0.write();
    T.write();
    U.write();
    Uf.write();

    forAll(V,c){V[c] = pMesh.V()[c];}
    V.write();
    forAll(Mass,c){Mass[c] = T.mesh().V()[c]*T[c];}
    Mass.write();
    dV.write();
    
    Info<< "End\n" << endl;

    return(0);
}

// ************************************************************************* //
