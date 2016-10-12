/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2013 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    simpleScalarTransportFoam

Description
    Solves a transport equation for a passive scalar

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "monitorFunction.H"
#include "faceToPointReconstruct.H"
#include "setInternalValues.H"
#include "meshToMesh0.H"
#include "processorFvPatch.H"
#include "MapMeshes.H"
#include "fvMesh.H"
#include "StreamFunctionAt.H"
#include "fvcMeshPhi.H"
#include "pointsToTerrainFollowing.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"
    Info().precision(16);
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

        // Open control dictionary
    IOdictionary controlDict
    (
        IOobject
        (
            "AFPDict", runTime.system(), runTime,
            IOobject::MUST_READ_IF_MODIFIED
        )
    );
    
    // The monitor funciton
    autoPtr<monitorFunction> monitorFunc(monitorFunction::New(controlDict));

    const scalar dx = mesh.bounds().span().x();
    const scalar dy = mesh.bounds().span().y();
    const scalar dz = mesh.bounds().span().z();
    scalar ztop = dz;
    Info << "dx = " << dx << endl;
    Info << "dy = " << dy << endl;
    Info << "dz = " << dz << endl;

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
    //const scalar orog_height(readScalar(initDict.lookup("orog_height")));
    //const scalar orog_start(readScalar(initDict.lookup("orog_start")));
    //const scalar orog_stop(readScalar(initDict.lookup("orog_stop")));
    const dimensionedScalar meshPeriod(initDict.lookup("meshPeriod"));

    const dimensionedScalar Gamma(controlDict.lookup("Gamma"));
    const scalar conv = readScalar(controlDict.lookup("conv"));
    const int nCorr = readLabel(mesh.solutionDict().lookup("nCorrectors"));

    //#include "createFields.H"
    #include "createMovingMeshFields.H"
    #include "refineMesh.H"
    rMesh.write();
    pointField points=rMesh.points();
    pointsToTerrainFollowing(points,ztop);
    pMesh.movePoints(points);
    pMesh.write();
    runTime.write();
    
    while (runTime.loop())
        {
            Info<< "Time = " << runTime.timeName() << ":" << nl << endl;
            
            #include "refineMesh.H"
            pointField points=rMesh.points();
            pointsToTerrainFollowing(points,ztop);
            pMesh.movePoints(points);
            //runTime++;
            runTime.write();

        }
    
    return 0;
}


// ************************************************************************* //
