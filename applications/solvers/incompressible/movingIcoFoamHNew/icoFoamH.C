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
    icoFoamH

Description
    C-grid solver for incompressible Euler equations using Hodge operator
    for curl-free pressure gradients. Note, Coriolis force not yet
    implemented

\*---------------------------------------------------------------------------*/

#include "HodgeOps.H"
#include "fvCFD.H"
#include "meshToMesh0.H"
#include "monitorFunction.H"
#include "faceToPointReconstruct.H"
#include "setInternalValues.H"
#include "fvMesh.H"
#include "fvcMeshPhi.H"

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

    const scalar dx = mesh.bounds().span().x();
    
    
    #include "orthogonalBoundariesPMesh.H"
    //#include "readEnvironmentalProperties.H"
    HodgeOps H(pMesh);
    #define dt runTime.deltaT()
    const scalar tau=2.0*Foam::constant::mathematical::pi;
    #include "createFields.H"
    #include "createMovingMeshFields.H"
    
    const dictionary& itsDict = pMesh.solutionDict().subDict("iterations");
    const int nCorr = itsDict.lookupOrDefault<int>("nCorrectors", 1);
    const int nNonOrthCorr =
        itsDict.lookupOrDefault<int>("nNonOrthogonalCorrectors", 0);
    //    const scalar offCentre = readScalar(pMesh.schemesDict().lookup("offCentre"));
    //const Switch uPredictor = itsDict.lookup("uPredictor");
    const Switch setReferenceP = pMesh.solutionDict().lookup("setReferenceP");


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
    
    
    const dimensionedVector constantMeshVelocity(initDict.lookup("constantMeshVelocity"));
    const dimensionedScalar meshPeriod(initDict.lookup("meshPeriod"));    

    IOdictionary monDict
        (
         IOobject
         (
          "monitorDict",
          mesh.time().constant(),
          mesh,
          IOobject::MUST_READ,
          IOobject::NO_WRITE
          )
         );
    
    
    const dimensionedScalar m1(monDict.lookup("m1"));
    const dimensionedScalar m2(monDict.lookup("m2"));
    const int nSmooth = monDict.lookupOrDefault<int>("nSmooth", 1);

    const Switch MArefinement = monDict.lookup("MArefinement");
    
    Info << "Max velocity u= " << max(mag(u)) << endl;
    Info << "Max vorticity = " << max(mag(q)) << endl; 
    #include "monitorCalc.H"
    monitorP.write();
    #include "monitorMap.H"
    monitorM.write();

    #include "setMovingMeshFields.H"


    

    #include "refineMesh.H"
    rMesh.write();
    //meshUpoints=rMesh.points();pMesh.movePoints(meshUpoints);pMesh.write();
    meshUpoints=pMesh.points();
    
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nStarting time loop\n" << endl;
    Info<< "PABe = " << PABe.value() << endl;
    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        //forAll(meshUpoints,point){
        //    meshUpoints[point] += dt.value()*constantMeshVelocity.value();
        //}
        #include "refineMesh.H"
        meshUpoints=rMesh.points();
        Info << "Total mesh movement = " << sumMag(pMesh.points()-meshUpoints) << endl;
        Info << "Max mesh movement = " << max(mag(pMesh.points()-meshUpoints)) << endl;
        #include "adjustMovementForCourantNo.H"
        #include "pEqn.H"

        Info << "Max velocity u= " << max(mag(u)) << endl;
        Info << "Max vorticity = " << max(mag(q)) << endl;
        
        #include "monitorCalc.H"
        #include "monitorMap.H"
        
        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }
    
    runTime.writeAndEnd();
    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
