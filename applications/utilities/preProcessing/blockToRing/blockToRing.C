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
    blockToRing

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

    Info<< "Reading initial conditions\n" << endl;

       

    
    // pointScalarField x = mesh.points().x();
    // pointScalarField y = mesh.points().y();
    // pointScalarField z = mesh.points().z();
    const scalar tau = 2.0*Foam::constant::mathematical::pi;
    const scalar inner_radius = 1.9;
    const scalar outer_radius = 2.0;
    scalar r = 0.0;
    scalar theta = 0.0;

    pointField targetPoints = mesh.points();

    //Info << "Before doing anything, the mesh volumes are: " << mesh.V();
    
    forAll(mesh.points(),point){
        scalar x = mesh.points()[point].x();
        scalar y = mesh.points()[point].y();
        //Info << "hello my name is point " << point << " "<< x << endl;
        theta = tau*x;
        if( y > 0.5 ) {
            r = inner_radius;
        } else {
            r = outer_radius;
        }
        targetPoints[point].x() = r*Foam::cos(theta);
        targetPoints[point].y() = r*Foam::sin(theta);
        
    }
    mesh.movePoints(targetPoints);



    //Info << "calling patchShell" << endl;
    patchShell(mesh,inner_radius,outer_radius);
    //Info << "called patchShell" << endl;

    Info << "After moving the points, the mesh volumes are: " << mesh.V();
    mesh.write();
    return(0);
}

// ************************************************************************* //
