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
#include "faceToPointReconstruct.H"

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
    
    #include "createFields.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nCalculating scalar transport\n" << endl;

    #include "CourantNo.H"

    const scalar deltaT = runTime.time().deltaT().value();
    meshUpoints *=deltaT;
    
    Info << "Max T = " << max(T) << " min T = " << min(T) << endl;
    Info << "Total T = " << fvc::domainIntegrate(T) << endl;
    
    Info << "Total volume of the mesh = " << sum(mesh.V()) << endl;
    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        rMesh.movePoints(rMesh.points() + meshUpoints);

        if( !phi().mesh().moving() ){Info << "The mesh is not moving." << endl;}
     
        fvc::makeRelative(phi,U);
        solve(fvm::ddt(T) + fvc::div(phi, T));
        fvc::makeAbsolute(phi,U);


        Info << "Max T = " << max(T) << " min T = " << min(T) << endl;
        Info << "Total T = " << fvc::domainIntegrate(T) << endl;


        runTime.write();
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
