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

#include "monitorFunctionSumGaussians.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "VectorSpaceFunctions.H"
#include "fvcLaplacian.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(monitorFunctionSumGaussians, 0);
addToRunTimeSelectionTable(monitorFunction, monitorFunctionSumGaussians, dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

monitorFunctionSumGaussians::monitorFunctionSumGaussians
(
    const IOdictionary& dict
)
:
    monitorFunction(dict),
    backgroundValue_(dict.lookup("backgroundValue")),
    gaussians_(dict.lookup("gaussians")),
    monScale_(dict.lookup("monScale")),
    monMax_(dict.lookup("monMax")),
    nMonSmooth_(readLabel(dict.lookup("nMonSmooth")))
{}

// * * * * * * * * * * * * * Member Functions * * * * * * * * * * * * * * //

tmp<volScalarField> monitorFunctionSumGaussians::map
(
    const fvMesh& newMesh,
    const volScalarField& oldMonitor
) const
{
    tmp<volScalarField> tMon
    (
        new volScalarField
        (
            IOobject("monitor", newMesh.time().timeName(), newMesh),
            newMesh,
            backgroundValue_*monScale_
        )
    );
    volScalarField& mon = tMon.ref();
    
    forAll(gaussians_, ig)
    {
        mon += monScale_*gaussians_[ig].field(newMesh);
    }
    
    mon = min(mon, monMax());
    
    // Apply laplacian smoothing
    for(label i = 0; i < nMonSmooth_; i++)
    {
        mon += 0.25*fvc::laplacian(1/sqr(newMesh.deltaCoeffs()), mon);
    }
    
    return tMon;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

tmp<surfaceVectorField> monitorFunctionSumGaussians::grad
(
    const fvMesh& newMesh,
    const surfaceVectorField& oldMonitor
) const
{
    tmp<surfaceVectorField> tMon
    (
        new surfaceVectorField
        (
            IOobject
            (
                "gradc_m",
                newMesh.time().timeName(),
                newMesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            newMesh,
            dimensionedVector("zero", dimless/dimLength, vector::zero)
        )
    );
//    surfaceVectorField& mon = tMon.ref();
    
    return tMon;
}

} // End namespace Foam

// ************************************************************************* //
