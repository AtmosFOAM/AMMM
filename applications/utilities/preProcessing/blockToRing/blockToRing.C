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

    const polyPatchList& patches = mesh.boundaryMesh();
    forAll(patches, patchi)
    {
        const polyPatch& patch = patches[patchi];
        Info << "hello I am patch " << patch.name() << endl;
        if(patch.name() == "outerShell")
        {
            Info << "da dadadadad" << endl;

            const labelList& meshPoints = patch.meshPoints();
            forAll(meshPoints, ip)
            {
                point& meshPoint = targetPoints[meshPoints[ip]];
                scalar x = meshPoint.x();
                scalar y = meshPoint.y();
                scalar r = Foam::sqrt(sqr(x) + sqr(y));
                meshPoint.x() = 10*outer_radius*x/r;
                meshPoint.y() = 10*outer_radius*y/r;
            }
        }
        else if(patches[patchi].name() == "innerShell")
        {
            Info << "do doododododod" << endl;
        }
    }

    mesh.movePoints(targetPoints);
    mesh.write();

    return(0);
}

// ************************************************************************* //
