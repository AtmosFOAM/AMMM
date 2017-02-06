/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2004 OpenCFD Ltd.
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
    Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Application
    movingshallowWaterFoamH

Description
    Transient semi-implicit solver for inviscid shallow-water equations in
    flux form with rotation on moving mesh. Uses the beta plane approximation and a C-grid.
    The mesh movement is solving by the Monge-Ampere equation


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

    // Read in and create variables and fields
    HodgeOps H(rMesh);
    #include "readEnvironmentalProperties.H"
    #include "createFields.H"
    #include "createMovingMeshFields.H"
    
    if (reMeshOnly)
    {
        #include "refineMesh.H"
        rMesh.write();
        mmPhi.write();
        monitorR.write();
        Info << "Created new rMesh. End\n" << endl;
        return 0;
    }
    
    // Read in solver options
    const dictionary& itsDict = rMesh.solutionDict().subDict("iterations");
    const int nCorr = readLabel(itsDict.lookup("nCorrectors"));;
    const int nNonOrthCorr = readLabel(itsDict.lookup("nNonOrthogonalCorrectors"));
    const scalar offCentre = readScalar(rMesh.schemesDict().lookup("offCentre"));

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nStarting time loop\n" << endl;
    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << endl;

        if (!fixedMesh)
        {
            #include "monitorCalc.H"
            #include "refineMesh.H"
            // Update Coriolis fields and geometry things
            twoOmegaf = (f0 + beta * rMesh.Cf().component(1))*OmegaHat;
            two_dxOmega = H.delta() ^ twoOmegaf;
            // Extra geometry for Hodge operators on the new mesh
            HodgeOps H(rMesh);
            faceVol = 1/3.*(H.delta() & rMesh.Sf());
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

    return(0);
}


// ************************************************************************* //
