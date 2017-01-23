/*---------------------------------------------------------------------------*\
 =========                   |
 \\      /   F ield          | OpenFOAM: The Open Source CFD Toolbox
  \\    /    O peration      |
   \\  /     A nd            | Copyright (C) 1991-2007 OpenCFD Ltd.
    \\/      M anipulation   |
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
    MAmesh_AFP



Description
    Solves the Monge-Ampere equation to move a mesh based on a monitor
    function defined on the original mesh by using the Adaptive Linearisation

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "monitorFunction.H"
#include "faceToPointReconstruct.H"
#include "setInternalValues.H"
#include "fvcPosDefCof.H"
#include "fvcDet.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{
    argList::noParallel();
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createMesh.H"
    // Create the mesh to be moved
    fvMesh rMesh
    (
        Foam::IOobject
        (
            "rMesh", runTime.timeName(), runTime, IOobject::MUST_READ
        )
    );

    // Open control dictionary
    IOdictionary controlDict
    (
        IOobject
        (
            args.executable() + "Dict", runTime.system(), runTime,
            IOobject::MUST_READ_IF_MODIFIED
        )
    );
    const int maxIters = readLabel(controlDict.lookup("maxIters"));
    
    // The monitor funciton
    autoPtr<monitorFunction> monitorFunc(monitorFunction::New(controlDict));
    
    #include "createFields.H"

    // Outer iterations to solve the Monge-Ampere eqn
    bool converged = false;
    for(label iter = 0; iter < maxIters && !converged; iter++)
    {
        Info<< "Monge Ampere iteration = " << iter << endl;

        // The matrix of co-factors for linearising the MA eqn
        cofacA = fvc::posDefCof(Hessian + tensor::I);

        // Calculate the source terms for the MA equation
        source = detHess - equiDistMean/monitorNew;

        // Setup and solve the MA equation to find Psi(t+1) 
        laplacianAPhi = fvc::laplacian(cofacA, Phi);
        for (int iCorr = 0; iCorr < 2; iCorr++)
        {
            fvScalarMatrix PhiEqn
            (
                fvm::laplacian(cofacA, Phi)
              - laplacianAPhi
              + source
            );
            PhiEqn.setReference(0, scalar(0));
            solverPerformance sp = PhiEqn.solve();
            if (iCorr == 0) converged = sp.nIterations() <= 1;
        }

        // Calculate gradient of Phi on faces and correct the normal component
        gradPhif = fvc::interpolate(fvc::grad(Phi));
        gradPhif += (fvc::snGrad(Phi) - (gradPhif & mesh.Sf())/mesh.magSf())
                    *mesh.Sf()/mesh.magSf();

        // Map gradPhi onto vertices in order to create the new mesh
        pointVectorField gradPhiP
             = fvc::faceToPointReconstruct(fvc::snGrad(Phi));
        rMesh.movePoints(mesh.points() + gradPhiP);

        // finite difference Hessian of Phi and its determinant
        Hessian = fvc::grad(gradPhif);
        detHess = fvc::det(Hessian + tensor::I);

        // map to or calculate the monitor function on the new mesh
        monitorR = monitorFunc().map(rMesh, monitor);
        setInternalValues(monitorNew, monitorR);

        // mean equidistribution, c
        equiDistMean = fvc::domainIntegrate(detHess)
                       /fvc::domainIntegrate(1/monitorNew);
    }
    
    if (!converged)
    {
        WarningIn("MAmesh_AFP") << "Not converged in " << maxIters
            << " iterations" << endl;
    }
    runTime.write();
    rMesh.write();
    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
