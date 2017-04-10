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

#include "monitorFunctionFromVorticity.H"
#include "fvcCurlf.H"
#include "addToRunTimeSelectionTable.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(monitorFunctionFromVorticity, 0);
addToRunTimeSelectionTable(monitorFunctionFrom, monitorFunctionFromVorticity, dictionary);


// * * * * * * * * * Protected Member Functions * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField> Foam::monitorFunctionFromVorticity::monitorBase
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
            magSqr(fvc::curl(Uf))
        )
    );

    return tmonitor;
}


Foam::tmp<Foam::volScalarField> Foam::monitorFunctionFromVorticity::baseToMonitor
(
    const volScalarField& b
) const
{
    const dimensionedScalar monBaseScale = 1/(monBaseMax() - monBaseMin());
    //const dimensionedScalar monBaseMid = 0.5*(monBaseMin() + monBaseMax());

    const fvMesh& mesh = b.mesh();
    tmp<volScalarField> tmonitor
    (
        new volScalarField
        (
            IOobject(name(), b.instance(), mesh),
            sqrt(sqr(monBaseScale)*b + 1)
        )
    );
    tmonitor.ref() = min(tmonitor.ref(), monitorMax());

    return tmonitor;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::monitorFunctionFromVorticity::monitorFunctionFromVorticity
(
    const IOdictionary& dict
)
:
    monitorFunctionFrom(dict),
    monBaseMin_(dict.lookup("monitorBaseMin")),
    monBaseMax_(dict.lookup("monitorBaseMax")),
    monitorMax_(readScalar(dict.lookup("monitorMax")))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::monitorFunctionFromVorticity::~monitorFunctionFromVorticity()
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
