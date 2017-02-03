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

#include "monitorFunctionFromNone.H"
#include "addToRunTimeSelectionTable.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(monitorFunctionFromNone, 0);
addToRunTimeSelectionTable(monitorFunctionFrom, monitorFunctionFromNone, dictionary);


// * * * * * * * * * Protected Member Functions * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField> Foam::monitorFunctionFromNone::monitorBase
(
    const surfaceVectorField& Uf
) const
{
    const fvMesh& mesh = Uf.mesh();
    tmp<volScalarField> tmonitor
    (
        new volScalarField
        (
            IOobject(name(), Uf.instance(), mesh),
            mesh,
            dimensionedScalar("ones", dimless, scalar(1))
        )
    );

    return tmonitor;
}

Foam::tmp<Foam::volScalarField> Foam::monitorFunctionFromNone::baseToMonitor
(
    const volScalarField& b
) const
{
    const fvMesh& mesh = b.mesh();
    tmp<volScalarField> tmonitor
    (
        new volScalarField
        (
            IOobject(name(), b.instance(), mesh),
            mesh,
            dimensionedScalar("ones", dimless, scalar(1))
        )
    );

    return tmonitor;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::monitorFunctionFromNone::monitorFunctionFromNone
(
    const IOdictionary& dict
)
:
    monitorFunctionFrom(dict)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::monitorFunctionFromNone::~monitorFunctionFromNone()
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
