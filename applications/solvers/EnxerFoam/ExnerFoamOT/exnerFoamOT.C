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
    exnerFoamOT

Description
    Transient Solver for buoyant, inviscid, incompressible, non-hydrostatic flow
    using a simultaneous solution of Exner, theta and phi. RC version uses
    co-located Rhie and Chow discretisation.
    OT uses a moving mesh solved using optimal transport.
    Orography is implemented using terrain following coordinates

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "ExnerTheta.H"
#include "OFstream.H"
#include "pimpleControl.H"
#include "dynamicFvMesh.H"
#include "CorrectPhi.H"
#include "terrainFollowingTransform.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addBoolOption("reMeshOnly", "Re-mesh then stop, no fluid flow");
    argList::addBoolOption("fixedMesh", "run on polyMesh and do not modify");
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createDynamicFvMesh.H"
    #include "createControl.H"
    #include "readEnvironmentalProperties.H"
    #include "readThermoProperties.H"
    dimensionedScalar nu(envProperties.lookup("nu"));
    #define dt runTime.deltaT()
    #include "createFields.H"
    #include "createMountain.H"
    
    const Switch reMeshOnly = args.optionFound("reMeshOnly");
    const Switch fixedMesh = args.optionFound("fixedMesh");

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    if (reMeshOnly)
    {
        Uf = fvc::interpolate(U);
        #include "flattenOrography.H"
        mesh.update();
        #include "raiseOrography.H"
        runTime.writeAndEnd();
    }

    Info<< "\nStarting time loop\n" << endl;

    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        #include "compressibleCourantNo.H"

        if (!fixedMesh)
        {
            Uf = fvc::interpolate(U);
            #include "flattenOrography.H"
            mesh.update();
            #include "raiseOrography.H"
        }

        while (pimple.loop())
        {
            #include "AEqn.H"
            #include "UEqn.H"
            #include "rhoThetaEqn.H"
            // Exner and momentum equations
            #include "exnerEqn.H"
        }
        #include "rhoThetaEqn.H"
        
        #include "compressibleContinuityErrs.H"

        dimensionedScalar totalHeatDiff = fvc::domainIntegrate(theta*rho)
                                        - initHeat;
        Info << "Heat error = " << (totalHeatDiff/initHeat).value() << endl;

        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
