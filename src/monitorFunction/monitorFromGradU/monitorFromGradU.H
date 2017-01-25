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
    Foam::functionObjects::monitorFromGradU

Group
    grpFieldFunctionObjects

Description
    Calculates the monitor function from the gradient of the velocity as
    sqrt(1 + monScale*magSqr(gradU))

See also
    Foam::functionObjects::fieldExpression
    Foam::functionObjects::fvMeshFunctionObject

SourceFiles
    monitorFromGradU.C

\*---------------------------------------------------------------------------*/

#ifndef functionObjects_monitorFromGradU_H
#define functionObjects_monitorFromGradU_H

#include "fieldExpression.H"
#include "dimensionedTypes.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{

/*---------------------------------------------------------------------------*\
                          Class monitorFromGradU Declaration
\*---------------------------------------------------------------------------*/

class monitorFromGradU
:
    public fieldExpression
{
    // Private data
        //- The scaling of the magSqr of gradU
        const dimensionedScalar monScale_;
        
        //- Number of iterations of laplacian smoothing
        const label nMonSmooth_;

    // Private Member Functions

        //- Calculate the monitor function field and return true if successful
        virtual bool calc();


public:

    //- Runtime type information
    TypeName("monitorFromGradU");


    // Constructors

        //- Construct from Time and dictionary
        monitorFromGradU
        (
            const word& name,
            const Time& runTime,
            const dictionary& dict
        );


    //- Destructor
    virtual ~monitorFromGradU();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace functionObjects
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
