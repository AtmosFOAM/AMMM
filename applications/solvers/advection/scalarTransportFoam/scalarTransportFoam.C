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
    scalarTransportFoam

Description
    Solves a transport equation for a passive scalar using RK2 time-stepping on an
    r-adaptive mesh

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "faceToPointReconstruct.H"
#include "setInternalValues.H"
#include "fvMesh.H"
#include "fvcMeshPhi.H"
#include "fvcDet.H"
#include "fvcPosDefCof.H"
#include "monitorFunctionFrom.H"
#include "velocityField.H"
#include "terrainFollowingTransform.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
using namespace Foam;

int main(int argc, char *argv[])
{
    argList::addBoolOption("reMeshOnly", "Re-mesh then stop, no fluid flow");
    argList::addBoolOption("fixedMesh", "run on rMesh and do not modify");
    argList::addBoolOption("colinParameter", "run with the Colin parameter A");
    #include "setRootCase.H"
    #include "createTime.H"
    #define dt runTime.deltaT()
    #include "createMesh.H"

    const Switch reMeshOnly = args.optionFound("reMeshOnly");
    const Switch fixedMesh = args.optionFound("fixedMesh");
    const Switch colinParameter = args.optionFound("colinParameter");

    // Create the moving mesh
    fvMesh rMesh
    (
        Foam::IOobject
        (
            "rMesh", runTime.timeName(), runTime,
            IOobject::MUST_READ, IOobject::AUTO_WRITE
        )
    );
    // Create the mesh which includes orography
    fvMesh pMesh
    (
        Foam::IOobject
        (
            "pMesh", runTime.timeName(), runTime,
            IOobject::MUST_READ, IOobject::AUTO_WRITE
        )
    );

    #include "createFields.H"
    #include "createMovingMeshFields.H"
    #include "createMountain.H"

    const dimensionedScalar maxMeshVelocity
    (
        mesh.solutionDict().lookup("maxMeshVelocity")
    );
    const dimensionedScalar maxMeshMovement = maxMeshVelocity*dt;

    if (reMeshOnly)
    {
        #include "refineMesh.H"
        #include "raiseOrography.H"
        rMesh.write();
        pMesh.write();
        mmPhi.write();
        monitorP.write();
        Info << "Created new rMesh and pMesh. End\n" << endl;
        return 0;
    }
    // Read the number of iterations each time-step
    const int nRKstages = readLabel(pMesh.solutionDict().lookup("nRKstages"));
    // The off-centering of the time-stepping scheme
    //const scalar offCentre = readScalar(pMesh.schemesDict().lookup("offCentre"));

    IOdictionary dict
    (
        IOobject
        (
            "advectionDict", pMesh.time().system(), pMesh,
            IOobject::READ_IF_PRESENT, IOobject::NO_WRITE
        )
    );
    autoPtr<velocityField> v = velocityField::New(dict);

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    #include "diagnosticsInit.H"

    Info<< "\nStarting time loop\n" << endl;
    while (runTime.loop())
    {
        Info<< "\nTime = " << runTime.timeName() << endl;

        if (!fixedMesh)
        {
            #include "monitorCalc.H"
            #include "refineMesh.H"
            #include "raiseOrography.H"

            // Update fluxes for the new mesh
            v->applyTo(phiR);
            setInternalAndBoundaryValues(phi, phiR);
            U = fvc::reconstruct(phi);
            //volRatio.field() = pMesh.V0()/pMesh.V();
            //A  = volRatio + dt*fvc::div(pMesh.phi());
        }

        #include "fluidEqns.H"

        #include "diagnostics.H"

        Info << "T goes from " << min(T).value() << " to " << max(T).value()
             << endl;
        Info << "uniT goes from " << min(uniT).value() << " to "
             << max(uniT).value() << endl;

        runTime.write();

    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
