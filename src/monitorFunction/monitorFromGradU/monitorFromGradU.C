/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2014-2016 OpenFOAM Foundation
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

#include "monitorFromGradU.H"
#include "fvcCurl.H"
#include "fvcLaplacian.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
    defineTypeNameAndDebug(monitorFromGradU, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        monitorFromGradU,
        dictionary
    );
}
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

bool Foam::functionObjects::monitorFromGradU::calc()
{
    if (foundObject<surfaceVectorField>(fieldName_))
    {
        const surfaceVectorField& Uf
            = lookupObject<surfaceVectorField>(fieldName_);
        const fvMesh& mesh = Uf.mesh();
        tmp<volScalarField> tmonitor
        (
            new volScalarField
            (
                IOobject(resultName_, Uf.instance(), mesh),
                sqrt(monScale_*magSqr(fvc::grad(Uf)) + 1)
            )
        );
        volScalarField& monitor = tmonitor.ref();
        // Smooth the monitor function
        for(int iSmooth = 0; iSmooth < nMonSmooth_; iSmooth++)
        {
            monitor += 0.25*fvc::laplacian(1/sqr(mesh.deltaCoeffs()), monitor);
        }

        return store(resultName_, tmonitor);
    }
    else
    {
        return false;
    }

    return true;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjects::monitorFromGradU::monitorFromGradU
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    fieldExpression(name, runTime, dict, "Uf"),
    monScale_(dict.lookup("monScale")),
    nMonSmooth_(readLabel(dict.lookup("nMonSmooth")))
{
    setResultName(typeName, "Uf");
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::functionObjects::monitorFromGradU::~monitorFromGradU()
{}


// ************************************************************************* //
