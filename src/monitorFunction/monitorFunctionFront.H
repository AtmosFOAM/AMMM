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

Class
    Foam::monitorFunctionFront

Description
    Defintes a moving monitor function with a Gaussian disctribution

SourceFiles
    monitorFunctionFront.C

\*---------------------------------------------------------------------------*/

#ifndef monitorFunctionFront_H
#define monitorFunctionFront_H

#include "monitorFunction.H"
#include "volFieldsFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class monitorFunctionFront Declaration
\*---------------------------------------------------------------------------*/

class monitorFunctionFront
:
    public monitorFunction
{
    // coefficients of the moving Gaussian function
    scalar p0_, m_, resolution_, width_, alpha_, gamma_, meshperiod_;
    
public:

    //- Runtime type information
    TypeName("monitorFunctionFront");

    // Constructors

        //- Construct from components
        monitorFunctionFront
        (
            const IOdictionary& dict
        );


    //- Destructor
    virtual ~monitorFunctionFront()
    {}


    // Member Functions

        // Access
        word name() const { return "monitor";}
        scalar p0() const { return p0_;}
        scalar gamma() const { return gamma_;}
        scalar alpha() const { return alpha_;}
        scalar m()  const { return m_;}
        scalar resolution() const { return resolution_;}
        scalar width() const { return width_;}
        scalar meshperiod() const { return meshperiod_;}

//        //- The scalarField of the monitor function at a Field of points
//        tmp<scalarField> map
//        (
//            const pointField& pts,
//            const scalarField& oldMonitor
//        ) const;

        //- The volScalarField of the monitor Function on the given mesh
        tmp<volScalarField> map
        (
            const fvMesh& newMesh,
            const volScalarField& oldMonitor
        ) const;

        tmp<surfaceVectorField> grad
        (
            const fvMesh& newMesh,
            const surfaceVectorField& oldMonitor
        ) const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
