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
    Foam::monitorFunctionFromTanhDiscriminant

Description
    Calculates the monitor function from the gradient of the velocity as
    0.5(m_x+1) + 0.5(m_x-1)tanh(monScale*(D + bm))

SourceFiles
    monitorFunctionFromTanhDiscriminant.C

\*---------------------------------------------------------------------------*/

#ifndef monitorFunctionFromTanhDiscriminant_H
#define monitorFunctionFromTanhDiscriminant_H

#include "monitorFunctionFrom.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Class monitorFunctionFromTanhDiscriminant Declaration
\*---------------------------------------------------------------------------*/

class monitorFunctionFromTanhDiscriminant
:
    public monitorFunctionFrom
{
protected:
    // Protected Member functions
    
        //- The function used to create the basis of the monitor function
        //- In this case the Q invariant of the velocity gradient
        tmp<volScalarField> monitorBase(const surfaceVectorField& Uf) const;

        //- The function used to convert the base function into the monitor
        tmp<volScalarField> baseToMonitor(const volScalarField& b) const;
public:

    //- Runtime type information
    TypeName("monitorFunctionFromTanhDiscriminant");


    // Constructors

        //- Construct from Time and dictionary
        monitorFunctionFromTanhDiscriminant
        (
            const IOdictionary& dict
        );


    //- Destructor
    virtual ~monitorFunctionFromTanhDiscriminant();

    // Member Functions
    
        //- Access
        word name() const { return "monitorFunctionFromTanhDiscriminant"; }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
