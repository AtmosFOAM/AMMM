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

#include "monitorFunctionFromTanhDiscriminant.H"
#include "fvcCurlf.H"
#include "addToRunTimeSelectionTable.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(monitorFunctionFromTanhDiscriminant, 0);
addToRunTimeSelectionTable(monitorFunctionFrom, monitorFunctionFromTanhDiscriminant, dictionary);


// * * * * * * * * * Protected Member Functions * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField> Foam::monitorFunctionFromTanhDiscriminant::monitorBase
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
            dimensionedScalar("", dimensionSet(0,0,-2,0,0), scalar(0))
        )
    );
    volTensorField gradU = fvc::grad(Uf);
    volScalarField& D = tmonitor.ref();
    forAll(D, cellI)
    {
        D[cellI] = pow(1/3.*invariantII(gradU[cellI]), 3)
                 + sqr(0.5*invariantIII(gradU[cellI]));
    }
    
    return tmonitor;
}


Foam::tmp<Foam::volScalarField> Foam::monitorFunctionFromTanhDiscriminant::baseToMonitor
(
    const volScalarField& b
) const
{
    const dimensionedScalar monBaseScale = 1/(monBaseMax() - monBaseMin());
    const dimensionedScalar monBaseMid = 0.5*(monBaseMin() + monBaseMax());

    const fvMesh& mesh = b.mesh();
    tmp<volScalarField> tmonitor
    (
        new volScalarField
        (
            IOobject(name(), b.instance(), mesh),
            0.5*(monitorMax()+1)
          + 0.5*(monitorMax()-1)*tanh(monBaseScale*(b - monBaseMid))
        )
    );

    return tmonitor;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::monitorFunctionFromTanhDiscriminant::monitorFunctionFromTanhDiscriminant
(
    const IOdictionary& dict
)
:
    monitorFunctionFrom(dict)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::monitorFunctionFromTanhDiscriminant::~monitorFunctionFromTanhDiscriminant()
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
