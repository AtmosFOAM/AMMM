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

Namespace
    Foam

Class
    Foam::monitorFunctionFrom

Description
    Abstract base class for the monitor function for an adaptive mesh
    simulation which calculates the monitor function from solution data

SourceFiles
    monitorFunctionFrom.C

\*---------------------------------------------------------------------------*/

#ifndef monitorFunctionFrom_H
#define monitorFunctionFrom_H

#include "IOdictionary.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "volFieldsFwd.H"
#include "fvMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
/*---------------------------------------------------------------------------*\
                           Class monitorFunctionFrom Declaration
\*---------------------------------------------------------------------------*/

class monitorFunctionFrom
:
    public IOdictionary
{
private:

    // Private Member Functions

        //- Disallow default bitwise copy construct
        monitorFunctionFrom(const monitorFunctionFrom&);

        //- Disallow default bitwise assignment
        void operator=(const monitorFunctionFrom&);

protected:

        //- The function used to create the basis of the monitor function
        virtual tmp<volScalarField> monitorBase(const surfaceVectorField& Uf)
            const = 0;

        //- The function used to convert the base function into the monitor
        virtual tmp<volScalarField> baseToMonitor(const volScalarField& b)
            const = 0;

public:

    //- Runtime type information
    TypeName("monitorFunctionFrom");


    // Declare run-time constructor selection table

        declareRunTimeSelectionTable
        (
            autoPtr,
            monitorFunctionFrom,
            dictionary,
            (
                const IOdictionary& dict
            ),
            (dict)
        );


    // Constructors
    
        //- Construct from dictionary
        monitorFunctionFrom(const IOdictionary& dict);
    
    // Selectors
    
        //- Return a reference to the selected monitorFunctionFrom
        static autoPtr<monitorFunctionFrom> New
        (
            const IOdictionary& dict
        );

    //- Destructor
    virtual ~monitorFunctionFrom(){};
    
    // Member Functions
    
        //- Access
        virtual word name() const = 0;

        //- The volScalarField of the monitor Function on the given mesh
        //- with smoothing
        tmp<volScalarField> evaluate
        (
            const surfaceVectorField& Uf
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
