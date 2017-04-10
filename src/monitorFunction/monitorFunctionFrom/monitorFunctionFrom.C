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

#include "monitorFunctionFrom.H"
#include "addToRunTimeSelectionTable.H"
#include "surfaceFields.H"
#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

defineTypeNameAndDebug(monitorFunctionFrom, 0);
defineRunTimeSelectionTable(monitorFunctionFrom, dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

monitorFunctionFrom::monitorFunctionFrom
(
    const IOdictionary& dict
)
:
    IOdictionary(dict),
    nMonSmooth_(readLabel(dict.lookup("nMonSmooth")))
{}


// * * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * //

autoPtr<monitorFunctionFrom> monitorFunctionFrom::New
(
    const IOdictionary& dict
)
{
    // get initial monitorFunctionFrom type, but do not register the dictionary
    // otherwise it is registered in the database twice
    const word monitorFunctionFromType(dict.lookup("monitorFunctionDynamic"));

    Info << "Selecting initial monitorFunctionDynamic type " << monitorFunctionFromType
         << endl;

    dictionaryConstructorTable::iterator cstrIter =
        dictionaryConstructorTablePtr_->find(monitorFunctionFromType);

    if (cstrIter == dictionaryConstructorTablePtr_->end())
    {
        FatalErrorIn
        (
            "monitorFunctionFrom::New"
            "("
                "const IOdictionary&, "
            ")"
        )   << "Unknown monitorFunctionFromType "
            << monitorFunctionFromType << nl << nl
            << "Valid types:" << endl
            << dictionaryConstructorTablePtr_->sortedToc()
            << exit(FatalError);
    }

    return autoPtr<monitorFunctionFrom>
    (
        cstrIter()(dict)
    );
}

// * * * * * * * * * * * * * *Member Functions * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField> Foam::monitorFunctionFrom::evaluate
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
            baseToMonitor(monitorBase(Uf))
        )
    );

    return tmonitor;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
