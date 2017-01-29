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

Class
    Foam::monitorFunctionFromNone

Description
    Calculates the monitor function as uniform (ones)

SourceFiles
    monitorFunctionFromNone.C

\*---------------------------------------------------------------------------*/

#ifndef monitorFunctionFromNone_H
#define monitorFunctionFromNone_H

#include "monitorFunctionFrom.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Class monitorFunctionFromNone Declaration
\*---------------------------------------------------------------------------*/

class monitorFunctionFromNone
:
    public monitorFunctionFrom
{
protected:
    // Protected Member functions
    
        //- The function used to create the basis of the monitor function
        tmp<volScalarField> monitorFunc(const surfaceVectorField& Uf) const;

public:

    //- Runtime type information
    TypeName("monitorFunctionFromNone");


    // Constructors

        //- Construct from Time and dictionary
        monitorFunctionFromNone
        (
            const IOdictionary& dict
        );


    //- Destructor
    virtual ~monitorFunctionFromNone();

    // Member Functions
    
        //- Access
        word name() const { return "monitorFunctionFromNone"; }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
