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
#include "faceToPointReconstructFake.H"
#include "meshToMesh0.H"
#include "processorFvPatch.H"
#include "MapMeshes.H"
#include "fvMesh.H"
#include "StreamFunctionAt.H"
//#include "HashTable.H"
//#include "fvPatchMapper.H"
//#include "scalarList.H"
//#include "className.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"
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
    #include "createFields.H"

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
    const scalar orog_height(readScalar(initDict.lookup("orog_height")));


    Info << "Calculating phi from u0 and h0" << endl;
    #include "StokesTheoremPhi.H"
    Info << "Reconstructing initial U from phi " << endl;
    U = fvc::reconstruct(phi);

    phi.write();
    U.write();
    const scalar pi = Foam::constant::mathematical::pi;
    const scalar tau=2.0*Foam::constant::mathematical::pi;
    //const bool no_subtract = false;
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nCalculating scalar transport\n" << endl;

    #include "CourantNo.H"

    const scalar deltaT = runTime.time().deltaT().value();
    meshUpoints *=deltaT;
    
    Info << "Max T = " << max(T) << " min T = " << min(T) << endl;
    Info << "Total T = " << fvc::domainIntegrate(T) << endl;
    
    Info << "Total volume of the mesh = " << sum(mesh.V()) << endl;
    Info << "Total depth of mesh = " << mesh.bounds().span().z() << endl;
    Info << "Total depth of  phi mesh = " << phi.mesh().bounds().span().z() << endl;

    Info << "mesh volumes = " << mesh.V();
    
    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        //meshUpoints = rMesh.points();
        forAll(meshUpoints,point){
            scalar theta_orig = Foam::atan2(mesh.points()[point].y(), mesh.points()[point].x());
            scalar r_orig = Foam::sqrt( sqr(mesh.points()[point].x()) + sqr(mesh.points()[point].y()) );
            scalar theta_new = theta_orig + Foam::sin(theta_orig)*Foam::sin(tau*runTime.value()/runTime.endTime().value())*tau/16;
            
            meshUpoints[point].x() = r_orig*Foam::cos(theta_new);
            meshUpoints[point].y() = r_orig*Foam::sin(theta_new);
        }
        #include "ringSetOrographyZ.H"
        
        rMesh.movePoints(meshUpoints);
        
        
        meshToMesh0 meshToMesh0Interp(mesh, rMesh);
        meshToMesh0::order mapOrder = meshToMesh0::INTERPOLATE;
        
        meshToMesh0Interp.interpolate(rh0,h0,mapOrder,eqOp<scalar>());
        
        rh0Faces = fvc::interpolate(rh0);
        #include "StokesTheoremPhi.H"
        U = fvc::reconstruct(phi);

        if( !phi().mesh().moving() ){Info << "The mesh is not moving." << endl;}
     
        fvc::makeRelative(phi,U);
        solve(fvm::ddt(T) + fvc::div(phi, T));
        //T = T.oldTime() - fvc::div(phi,T);
        //phiT = fvc::interpolate(T)*phi;
        fvc::makeAbsolute(phi,U);


        Info << "Max T = " << max(T) << " min T = " << min(T) << endl;
        Info << "Total T = " << fvc::domainIntegrate(T) << endl;
        Info << "Total orography = " << fvc::domainIntegrate(rh0) << endl;

        runTime.write();
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
