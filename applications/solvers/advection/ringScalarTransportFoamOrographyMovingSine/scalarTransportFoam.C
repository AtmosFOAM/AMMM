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
#include "meshToMesh0.H"
#include "processorFvPatch.H"
#include "MapMeshes.H"
#include "fvMesh.H"
#include "StreamFunctionAt.H"
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
    
    #include "createFields.H"

    Info<< "Reading initial conditions\n" << endl;

    IOdictionary initDict
    (
        IOobject
        (
            "initialConditions", mesh.time().constant(),mesh,IOobject::MUST_READ
        )
    );

    // Maximum jet velocity
    const dimensionedScalar u0(initDict.lookup("u0"));
    const scalar orog_height(readScalar(initDict.lookup("orog_height")));
    const dimensionedScalar meshPeriod(initDict.lookup("meshPeriod"));


    Info << "Calculating phi from u0 and h0" << endl;
    #include "StokesTheoremPhi.H"
    Info << "Reconstructing initial U from phi " << endl;
    U = fvc::reconstruct(phi);
    phi.oldTime();
    U.oldTime();

    Info << "Divergence of U = " << fvc::div(phi) << endl;
    
    phi.write();
    U.write();
    const scalar pi = Foam::constant::mathematical::pi;
    const scalar tau=2.0*Foam::constant::mathematical::pi;
    //const bool no_subtract = false;
    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nCalculating scalar transport\n" << endl;

    const scalar deltaT = runTime.time().deltaT().value();
    meshUpoints *=deltaT;
    
    Info << "Max T = " << max(T) << " min T = " << min(T) << endl;
    Info << "Total T = " << fvc::domainIntegrate(T) << endl;
    
    Info << "Total volume of the mesh = " << sum(mesh.V()) << endl;
    Info << "Total depth of mesh = " << mesh.bounds().span().z() << endl;
    Info << "Total depth of  phi mesh = " << phi.mesh().bounds().span().z() << endl;


    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << ":" << nl << endl;
        #include "CourantNo.H"

        //meshUpoints = rMesh.points();
        forAll(meshUpoints, point)
        {
            scalar theta_orig = Foam::atan2
            (
                mesh.points()[point].y(), mesh.points()[point].x()
            );
            scalar r_orig = Foam::sqrt
            (
                sqr(mesh.points()[point].x()) + sqr(mesh.points()[point].y())
            );
            scalar theta_new = theta_orig
                             + Foam::sin(theta_orig)
                             *Foam::sin
                             (
                                tau*runTime.value()/meshPeriod.value()
                             )*tau/16;
            
            meshUpoints[point].x() = r_orig*Foam::cos(theta_new);
            meshUpoints[point].y() = r_orig*Foam::sin(theta_new);
            meshUpoints[point].z() = rMesh.points()[point].z();
        }
        rMesh.movePoints(meshUpoints);
        #include "ringSetOrographyZ.H"
        pMesh.movePoints(meshUpoints);
//        #include "meshCourantNo.H"
        
//        meshToMesh0 meshToMesh0Interp(mesh, rMesh);
//        meshToMesh0::order mapOrder = meshToMesh0::INTERPOLATE;
//        meshToMesh0Interp.interpolate(rh0,h0,mapOrder,eqOp<scalar>());
        
        #include "StokesTheoremPhi.H"
        U = fvc::reconstruct(phi);

        if( !phi().mesh().moving() )
        {
            Info << "The mesh is not moving." << endl;
        }

        fvc::makeRelative(phi.oldTime(), U.oldTime());
        solve(fvm::ddt(T) + fvc::div(phi.oldTime(), T.oldTime()));

        Info << "Max T = " << max(T) << " min T = " << min(T) << endl;
        Info << "Total T = " << fvc::domainIntegrate(T) << endl;
        Info << "Mean T = " << sum(T)/T.size() << endl;
        Info << "Total orography = " << sum(rMesh.V())-sum(pMesh.V()) << endl;
        Info << "Vol pMesh = " << sum(pMesh.V()) << endl;

        runTime.write();
        return 0;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
