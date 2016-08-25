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
    shallowWaterFoamBeta_C_explicit

Description
    Transient explicit solver for inviscid shallow-water equations in
    flux form with rotation. Uses the beta plane approximation and a C-grid.
    The geometry needs to be in the x-y plane as rotation is assumed about
    the z-axis. Gravity waves and advection treated explicitly

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "faceToPointReconstruct.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"
    #include "readEnvironmentalProperties.H"
    #define dt runTime.deltaT()
    // Read solution controls
    const int nCorr = readLabel(mesh.solutionDict().lookup("nCorrectors"));
    const scalar offCentre = readScalar(mesh.schemesDict().lookup("offCentre"));
    #include "createFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    ofstream myfile;
    myfile.open (runTime.timeName()+"/data");
    forAll(h, i)
        {
            myfile << mesh.C()[i].x() << ' '
                   << h[i] << ' '
                   << hTotal[i] << ' '
                   << U[i].x() << '\n';
        }            
    myfile.close();
    myfile.open (runTime.timeName()+"/inth");
    myfile.precision(16);
    myfile << fvc::domainIntegrate(h).value()/(mesh.bounds().span().y()*mesh.bounds().span().z()) << "\n";
    myfile.close();
    

    
    Info<< "\nStarting time loop\n" << endl;
    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

#       include "CourantNo.H"

        // --- Outer loop

        for (int corr=0; corr<nCorr; corr++)
        {
            // Calculate the explicit source terms in the momentum equations
            // without Coriolis
            dFluxdt = -hf*gsnGradh0
                    - (fvc::interpolate(fvc::div(flux,U)) & mesh.Sf());
            
            // Crank Nicolson update of flux without implicit terms and
            // without Coriolis which needs to be balanced at the boundary
            fluxU = flux.oldTime() + runTime.deltaT()*
            (
                (1-offCentre)*dFluxdt.oldTime() + offCentre*dFluxdt
            );
            
            // Add components of the flux that can create flow over boundaries
            flux = fluxU - dt*offCentre*hf*g*fvc::snGrad(h)*mesh.magSf();
            if (rotating) flux -= dt*offCentre*hf*(fSfxk & Uf);

            Info<<"before make relative" << endl;
            Info<<"dim(flux) = " << flux.dimensions() << endl;
            Info<<"dim(meshU) = " << meshU.dimensions() << endl;
            dimensionedScalar rho("rho", dimLength, 1.0);
            Info<<"dim(rho) = " << rho.dimensions() << endl;
            surfaceScalarField asdf = fvc::meshPhi(rho,U);
            Info<<"dim(meshPhi) = " << asdf.dimensions() << endl;
            surfaceScalarField sflux = flux/rho;
            fvc::makeRelative(flux,h,U);
            Info<<"after make relative" << endl;
            // Solve the continuity equation
            fvScalarMatrix hEqn
            (
                fvm::ddt(h)
              + fvc::div((1-offCentre)*flux.oldTime() + offCentre*flux)
            );

            hEqn.solve();

            // update diagnostic fields
            hf = fvc::interpolate(h);
            U = fvc::reconstruct(flux/hf);
            Uf = fvc::interpolate(U);
        }
        // Rate of change of flux for next time-step (including all terms)
        dFluxdt -= g*hf*fvc::snGrad(h)*mesh.magSf();
        if (rotating) dFluxdt -= hf*(fSfxk & Uf);

        hTotal == h + h0;
        runTime.write();
        if( int(runTime.value()) % 250*40 == 0 ) {
            ofstream myfile;
            myfile.open (runTime.timeName()+"/data");
            myfile.precision(16);
            //int i;
            forAll(h, i)
                {
                    myfile << mesh.C()[i].x() << ' '
                           << h[i] << ' '
                           << hTotal[i] << ' '
                           << U[i].x() << '\n';
                }            
            myfile.close();

            myfile.open (runTime.timeName()+"/inth");
            myfile.precision(16);
            myfile << fvc::domainIntegrate(h).value()/(mesh.bounds().span().y()*mesh.bounds().span().z()) << "\n";
            myfile.close();
        }

        Info<< fvc::domainIntegrate(h)/(mesh.bounds().span().y()*mesh.bounds().span().z()) << endl;

        Info<< "\n    ExecutionTime = " << runTime.elapsedCpuTime() << " s\n"
            << endl;

        mesh.movePoints(mesh.points() + meshUpoints);
        Info<<"mesh.movePoints completed" << endl;
    }

    Info<< "\n end \n";

    return(0);
}


// ************************************************************************* //
