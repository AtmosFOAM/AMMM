/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2009 OpenCFD Ltd.
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
    movingIcoFoamH

Description
    C-grid solver for incompressible Euler equations using Hodge operator
    for curl-free pressure gradients on a moving mesh. Mesh calculated
    using optimal transport. Note, Coriolis force not yet implemented

\*---------------------------------------------------------------------------*/

#include "HodgeOps.H"
#include "fvCFD.H"
#include "faceToPointReconstruct.H"
#include "setInternalValues.H"
#include "fvMesh.H"
#include "fvcMeshPhi.H"
#include "fvcDet.H"
#include "fvcPosDefCof.H"
#include "fvcCurlf.H"
#include "monitorFunctionFrom.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
using namespace Foam;

int main(int argc, char *argv[])
{
    argList::addBoolOption("reMeshOnly", "Re-mesh then stop, no fluid flow");
    argList::addBoolOption("fixedMesh", "run on rMesh and do not modify");
    #include "setRootCase.H"
    #include "createTime.H"
    #define dt runTime.deltaT()
    #include "createMesh.H"
    
    const Switch reMeshOnly = args.optionFound("reMeshOnly");
    const Switch fixedMesh = args.optionFound("fixedMesh");

    // Create the moving mesh
    fvMesh rMesh
    (
        Foam::IOobject
        (
            "rMesh", runTime.timeName(), runTime,
            IOobject::MUST_READ, IOobject::AUTO_WRITE
        )
    );

    Info << "Mesh has normal direction" << flush;
    vector meshNormal = 0.5*(Vector<label>(1,1,1)-mesh.geometricD());
    meshNormal -= 2*meshNormal[1]*vector(0.,1.,0.);
    Info << meshNormal << endl;

    // Read in and create fields
    #include "createFields.H"
    #include "createMovingMeshFields.H"
    
    if (reMeshOnly)
    {
        #include "refineMesh.H"
        rMesh.write();
        mmPhi.write();
        Info << "Created new rMesh. End\n" << endl;
        return 0;
    }
    
    // Read in solver options
    const dictionary& itsDict = rMesh.solutionDict().subDict("iterations");
    const int nCorr = readLabel(itsDict.lookup("nCorrectors"));;
    const int nNonOrthCorr = readLabel(itsDict.lookup("nNonOrthogonalCorrectors"));
    const scalar offCentre = readScalar(rMesh.schemesDict().lookup("offCentre"));
    const Switch setReferenceP = rMesh.solutionDict().lookup("setReferenceP");

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nStarting time loop\n" << endl;
    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        if (!fixedMesh)
        {
            #include "monitorCalc.H"
            #include "refineMesh.H"
        }
        else
        {
            pointField newPoints = rMesh.points();
            rMesh.movePoints(newPoints);
        }
        #include "fluidEqns.H"
        
        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }
    
    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
