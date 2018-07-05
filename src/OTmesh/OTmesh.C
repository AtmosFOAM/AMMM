/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "OTmesh.H"
#include "addToRunTimeSelectionTable.H"
#include "fvCFD.H"
#include "faceToPointReconstruct.H"
#include "setInternalValues.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(OTmesh, 0);
    addToRunTimeSelectionTable(dynamicFvMesh, OTmesh, IOobject);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::OTmesh::OTmesh(const IOobject& io)
:
    dynamicFvMesh(io),
    cMesh_
    (
        IOobject
        (
            "cMesh", io.time().constant(), io.time(),
            IOobject::MUST_READ, IOobject::NO_WRITE
        )
    ),
    OTmeshCoeffs_
    (
        IOdictionary
        (
            IOobject
            (
                "OTmeshDict",
                time().system(),
                *this,
                IOobject::MUST_READ_IF_MODIFIED,
                IOobject::NO_WRITE,
                false
            )
        )
    ),
    maxMAiters_(readLabel(OTmeshCoeffs_.lookup("maxMAiters"))),
    maxMeshVelocity_(readScalar(OTmeshCoeffs_.lookup("maxMeshVelocity"))),
    meshRelax_(readScalar(OTmeshCoeffs_.lookup("meshRelax"))),
    meshDiffusion_(OTmeshCoeffs_.lookup("meshDiffusion")),
    meshPot_
    (
        IOobject
        (
            "meshPot", cMesh_.time().timeName(), cMesh_,
            IOobject::MUST_READ, IOobject::AUTO_WRITE
        ),
        cMesh_
    ),
    gradMeshPot_(fvc::interpolate(fvc::grad(meshPot_))),
    monitorFromName_(OTmeshCoeffs_.lookup("monitorFromName")),
    monitorFunc_(monitorFunctionFrom::New(OTmeshCoeffs_)),
    monitorP_
    (
        IOobject
        (
            "monitor",
            io.time().timeName(),
            *this,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        *this,
        dimensionedScalar("monitor", dimless, scalar(1))
    ),
    monitorC_
    (
        IOobject("monitor", io.time().timeName(), cMesh_),
        cMesh_,
        dimensionedScalar("monitor", dimless, scalar(1)),
        meshPot_.boundaryField().types()
    )
{
    gradMeshPot_ += 
    (
        fvc::snGrad(meshPot_) - (gradMeshPot_ & cMesh_.Sf())/cMesh_.magSf()
    )*cMesh_.Sf()/cMesh_.magSf();
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::OTmesh::~OTmesh()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::OTmesh::setMonitor()
{
    Info << "OTmesh::setMonitor: Monitor function goes from " << flush;

    const surfaceVectorField& Uf
    (
        lookupObject<surfaceVectorField>(monitorFromName_)
    );
    
    monitorP_ = monitorFunc_().evaluate(Uf);
    setInternalAndBoundaryValues(monitorC_, monitorP_);

    // Smoothing of the monitor function on the computational mesh
    if (meshDiffusion().value() > SMALL)
    {
        dimensionedScalar diffCoeff = meshDiffusion()/time().deltaT();
        monitorC_.oldTime() = monitorC_;
        fvScalarMatrix smoothEqn
        (
            fvm::ddt(monitorC_)
         == fvm::laplacian(diffCoeff, monitorC_, "monitor")
        );
        smoothEqn.solve();
    }

    setInternalAndBoundaryValues(monitorP_, monitorC_);

    Info << min(monitorC_).value() << " to " << max(monitorC_).value() << endl;
}

bool Foam::OTmesh::update()
{
    setMonitor();

    // Numerical solution of the Monge-Ampere equation to move the mesh
    bool converged = false;
    for(int nMAit = 0; nMAit < maxMAiters() && !converged; nMAit++)
    {
        // Hessian and its determinant
        volTensorField Hessian("Hessian", fvc::grad(gradMeshPot_));
        volScalarField detHess("detHess", fvc::det(Hessian + tensor::I));

        // The matrix A of co-factors
        volTensorField cofacA("cofacA", fvc::posDefCof(Hessian + tensor::I));

        // The mean equidistribution, c
        dimensionedScalar equiDistMean = fvc::domainIntegrate(detHess)
                                 /fvc::domainIntegrate(1/monitorC_);

        // Source term
        volScalarField source("source", detHess - equiDistMean/monitorC_);

        // The laplacian of (A,meshPot) at the old iteration
        volScalarField laplacianAPhi
        (
            "laplacianAPhi",
            fvc::laplacian(cofacA, meshPot_)
        );
    
        // Setup and solve the MA equation to find the new meshPot
        for (int iCorr = 0; iCorr < 2; iCorr++)
        {
            fvScalarMatrix meshPotEqn
            (
                fvm::laplacian(cofacA, meshPot_)
              - laplacianAPhi
              + source
            );
            meshPotEqn.setReference(0, scalar(0));
            solverPerformance sp = meshPotEqn.solve();
            if (iCorr == 0) converged = sp.nIterations() <= 2;
        }

        // The gradient of the mesh potential on faces
        gradMeshPot_ = fvc::interpolate(fvc::grad(meshPot_));
        gradMeshPot_ += 
        (
            fvc::snGrad(meshPot_) - (gradMeshPot_ & cMesh_.Sf())/cMesh_.magSf()
        )*cMesh_.Sf()/cMesh_.magSf();
    }

    // Map gradMeshPot onto vertices in order to create the new mesh
    pointVectorField gradMeshPotP
         = fvc::faceToPointReconstruct(fvc::snGrad(meshPot_));

    // Control the mesh velocity
    pointField newPoints = cMesh().points() + gradMeshPotP;
    vectorField meshMovement = newPoints - points();

    // Scale meshMovement so that mesh doesn't move more than maxMeshMovement
    scalar maxMM = max(mag(meshMovement));
    scalar maxMMallowed = maxMeshVelocity_*time().deltaT().value();
    if (maxMMallowed > 0 && maxMM > maxMMallowed)
    {
        meshMovement *= maxMMallowed/maxMM;
        Info << "Scaling mesh movement from maximum of " << maxMM
             << " to " << maxMMallowed << endl;
    }
    else if (meshRelax_ > SMALL && meshRelax_ < 1-SMALL)
    {
        meshMovement *= meshRelax_;
        Info << "Max mesh velocity = "
             << max(mag(meshMovement))/time().deltaT().value() << endl;
    }

    // Move the points
    fvMesh::movePoints(points() + meshMovement);

    // Correct the velocity boundary conditions
    volVectorField& U =
        const_cast<volVectorField&>(lookupObject<volVectorField>("U"));
    U.correctBoundaryConditions();

    return true;
}


// ************************************************************************* //
