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

#include "monitorFunctionFromLinQ.H"
#include "fvcCurlf.H"
#include "addToRunTimeSelectionTable.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(monitorFunctionFromLinQ, 0);
addToRunTimeSelectionTable(monitorFunctionFrom, monitorFunctionFromLinQ, dictionary);


// * * * * * * * * * Protected Member Functions * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField> Foam::monitorFunctionFromLinQ::monitorBase
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
    volScalarField& Q = tmonitor.ref();
    forAll(Q, cellI)
    {
        Q[cellI] = invariantII(gradU[cellI]);
    }
    
    Info << "Q goes from " << min(Q.internalField()).value() << " to "
         << max(Q.internalField()).value() << ", " << flush;

    return tmonitor;
}


Foam::tmp<Foam::volScalarField> Foam::monitorFunctionFromLinQ::baseToMonitor
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
            1 + (monitorMax()-1)*(b - monBaseMin())
                                /(monBaseMax()-monBaseMin())
        )
    );
    volScalarField& m = tmonitor.ref();
    forAll(m, cellI)
    {
        if (b[cellI] < monBaseMin().value()) m[cellI] = 1;
        else if(b[cellI] > monBaseMax().value()) m[cellI] = monitorMax();
    }
    
    Info << "\nb goes from " << min(b.internalField()).value() << " to "
         << max(b.internalField()).value()
         << " monitor goes from "
         << min(tmonitor.ref().internalField()).value() << " to "
         << max(tmonitor.ref().internalField()).value() << endl;

    return tmonitor;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::monitorFunctionFromLinQ::monitorFunctionFromLinQ
(
    const IOdictionary& dict
)
:
    monitorFunctionFrom(dict)
{}



// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::monitorFunctionFromLinQ::~monitorFunctionFromLinQ()
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
