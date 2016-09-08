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

using namespace Foam::constant::mathematical;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{

#   include "setRootCase.H"
#   include "createTime.H"
#   include "createMesh.H"

    // pointScalarField x = mesh.points().x();
    // pointScalarField y = mesh.points().y();
    // pointScalarField z = mesh.points().z();
    const scalar tau = 2.0*Foam::constant::mathematical::pi;
    const scalar inner_radius = 1.9;
    const scalar outer_radius = 2.0;
    scalar r = 0.0;
    scalar theta = 0.0;

    pointField targetPoints = mesh.points();
    
    forAll(mesh.points(),point){
        scalar x = mesh.points()[point].x();
        scalar y = mesh.points()[point].y();
        Info << "hello my name is point " << point << " "<< x << endl;
        theta = tau*x;
        if( y > 0.5 ) {
            r = inner_radius;
        } else {
            r = outer_radius;
        }
        targetPoints[point].x() = r*Foam::sin(theta);
        targetPoints[point].y() = r*Foam::cos(theta);
            
    }
    mesh.movePoints(targetPoints);
    
    mesh.write();
    Info<< "End\n" << endl;


    forAll(mesh.points(),point){
        
    }
    mesh.movePoints(targetPoints);

    
    const polyPatchList& patches = mesh.boundaryMesh();
    forAll(patches, patchi){
        Info << "hello I am patch " << patches[patchi].name() << endl;
        if(patches[patchi].name() == "outerShell"){
            Info << "da dadadadad" << endl;

            pointField patchTarget = patches[patchi].points();
            forAll(patchTarget, point){
                scalar x = patchTarget[point].x();
                scalar y = patchTarget[point].y();
                scalar r = Foam::sqrt( sqr(x) + sqr(y) );
                patchTarget[point].x() = 10*outer_radius*x/r;
                patchTarget[point].y() = 10*outer_radius*y/r;
            }
            Info << "patchTarget = " << patchTarget << endl;
            const pointField cpatchTarget = patchTarget;
            patches[patchi].movePoints(cpatchTarget);

        }    else if(patches[patchi].name() == "innerShell"){
            Info << "do doododododod" << endl;
        }
    }
    return(0);
}

// ************************************************************************* //
