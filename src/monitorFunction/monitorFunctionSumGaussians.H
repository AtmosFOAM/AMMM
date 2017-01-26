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
    Foam::monitorFunctionSumGaussians

Description
    Defintes an analytic form of the monitor function using the sum of some
    Gaussians. The final monitor function is 
    sqrt(1 + monScale*sumGaussians)
    and then laplacian smoothing is applied nMonSmooth times

SourceFiles
    monitorFunctionSumGaussians.C

\*---------------------------------------------------------------------------*/

#ifndef monitorFunctionSumGaussians_H
#define monitorFunctionSumGaussians_H

#include "monitorFunction.H"
#include "volFieldsFwd.H"
#include "gaussian.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                 Class monitorFunctionSumGaussians Declaration
\*---------------------------------------------------------------------------*/

class monitorFunctionSumGaussians
:
    public monitorFunction
{
    // Private data
        //- Background value
        dimensionedScalar backgroundValue_;
    
        //- a list of Gaussians
        List<gaussian> gaussians_;
    
        //- Scaling to apply to the square of the sum of the Gaussians
        dimensionedScalar monScale_;
        
        //- Number of applications of laplacian smoothing to the monitor
        label nMonSmooth_;
    
public:

    //- Runtime type information
    TypeName("monitorFunctionSumGaussians");

    // Constructors

        //- Construct from dictionary
        monitorFunctionSumGaussians
        (
            const IOdictionary& dict
        );


    //- Destructor
    virtual ~monitorFunctionSumGaussians()
    {}


    // Member Functions

        // Access
        word name() const { return "monitor"; }
        
        //- The Gaussian functions to be summed
        const List<gaussian>& gaussians() const { return gaussians_; }

        //- The background value to add to the sum of the Gaussians
        const dimensionedScalar& backgroundValue() const
             { return backgroundValue_;}
             
        //- Scaling to apply to the square of the sum of the Gaussians
        const dimensionedScalar& monScale() const { return monScale_; }
        
        //- Number of applications of laplacian smoothing to the monitor
        label nMonSmooth() { return nMonSmooth_; }

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
