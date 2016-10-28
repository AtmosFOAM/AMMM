/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2016 OpenFOAM Foundation
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
    icoFoam

Description
    Transient solver for incompressible, laminar flow of Newtonian fluids.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "pisoControl.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"

    pisoControl piso(mesh);

    #include "createFields.H"
    #include "initContinuityErrs.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nStarting time loop\n" << endl;

    while (runTime.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        #include "CourantNo.H"

        // Momentum predictor

        //set up the linear algebra for the momentum equation.  The flux of U, phi, is treated explicity
        //using the last known value of U.  
        
        fvVectorMatrix UEqn
        (
            fvm::ddt(U)
          + fvm::div(phi, U)
          - fvm::laplacian(nu, U)
        );

        if (piso.momentumPredictor())
        {
            // solve using the last known value of p on the RHS.  This gives us a velocity field that is
            // not divergence free, but approximately satisfies momentum.  See Eqn. 7.31 of Ferziger & Peric
            solve(UEqn == -fvc::grad(p));
        }

        // --- PISO loop
        while (piso.correct())
        {
            // from the last solution of velocity, extract the diag. term from the matrix and store the reciprocal
            // note that the matrix coefficients are functions of U due to the non-linearity of convection.
            volScalarField rAU(1.0/UEqn.A());
            volVectorField HbyA(constrainHbyA(rAU*UEqn.H(), U, p));

            // calculate the fluxes by dotting the interpolated velocity (to cell faces) with face normals
            // The ddtPhiCorr term accounts for the divergence of the face velocity field by taking out the 
            //  difference between the interpolated velocity and the flux.
            surfaceScalarField phiHbyA
            (
                "phiHbyA",
                fvc::flux(HbyA)
              + fvc::interpolate(rAU)*fvc::ddtCorr(U, phi)
            );


            // adjusts the inlet and outlet fluxes to obey continuity, which is necessary for creating a well-posed
            // problem where a solution for pressure exists.
            adjustPhi(phiHbyA, U, p);

            // Update the pressure BCs to ensure flux consistency
            constrainPressure(p, U, phiHbyA, rAU);

            // iteratively correct for non-orthogonality.  The non-orthogonal part of the Laplacian is calculated from the most recent
            // solution for pressure, using a deferred-correction approach.
            
            // Non-orthogonal pressure corrector loop
            while (piso.correctNonOrthogonal())
            {
                // Pressure corrector

                // set up the pressure equation
                fvScalarMatrix pEqn
                (
                    fvm::laplacian(rAU, p) == fvc::div(phiHbyA)
                );

                // in incompressible flow, only relative pressure matters.  Unless there is a pressure BC present,
                // one cell's pressure can be set arbitrarily to produce a unique pressure solution
                pEqn.setReference(pRefCell, pRefValue);

                pEqn.solve(mesh.solver(p.select(piso.finalInnerIter())));

                
                // on the last non-orthogonality correction, correct the flux using the most up-to-date pressure
                if (piso.finalNonOrthogonalIter())
                {
                    //The .flux method includes contributions from all implicit terms of the pEqn (the Laplacian)
                    phi = phiHbyA - pEqn.flux();
                }
            }// end of non-orthogonality looping

            #include "continuityErrs.H"
            
            // add pressure gradient to interior velocity and BC's.  Note that this pressure is not just a small
            // correction to a previous pressure, but is the entire pressure field.  Contrast this to the use of p'
            // in Ferziger & Peric, Eqn. 7.37.
            U = HbyA - rAU*fvc::grad(p);
            U.correctBoundaryConditions();
        }

        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
