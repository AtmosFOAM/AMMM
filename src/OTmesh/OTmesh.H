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
    Foam::OTmesh

Description
    A moving mesh with points caluclated from the solution of the
    optimal transport (OT) problem by numerical solution of the Monge-Ampere
    equation on a computational mesh

SourceFiles
    OTmesh.C

\*---------------------------------------------------------------------------*/

#ifndef OTmesh_H
#define OTmesh_H

#include "fvMesh.H"
#include "dynamicFvMesh.H"
#include "dictionary.H"
#include "pointIOField.H"
#include "fvcDet.H"
#include "fvcPosDefCof.H"
#include "fvcCurlf.H"
#include "monitorFunctionFrom.H"
#include "volFields.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class OTmesh Declaration
\*---------------------------------------------------------------------------*/

class OTmesh
:
    public dynamicFvMesh
{
    // Private data

        //- The computational mesh for sovling the MA equation
        fvMesh cMesh_;

        IOdictionary OTmeshCoeffs_;

        label maxMAiters_;
        scalar maxMeshVelocity_;
        scalar meshRelax_;
        scalar smoothCoeff_;
        
        // The mesh potential and its gradient
        volScalarField meshPot_;
        surfaceVectorField gradMeshPot_;
        
        // name of the vector field to use to calculate the monitor function
        word monitorFromName_;

        // The form of the monitor funciton
        autoPtr<monitorFunctionFrom> monitorFunc_;

        //- The monitor function on the physical and computational meshes
        volScalarField monitorP_;
        volScalarField monitorC_;


    // Private Member Functions

        //- Disallow default bitwise copy construct
        OTmesh(const OTmesh&);

        //- Disallow default bitwise assignment
        void operator=(const OTmesh&);


public:

    //- Runtime type information
    TypeName("OTmesh");


    // Constructors

        //- Construct from IOobject
        OTmesh(const IOobject& io);


    //- Destructor
    ~OTmesh();


    // Member Functions
    
        //- Access
        //- The computational mesh
        const fvMesh& cMesh() const { return cMesh_; }

        //- The maximum number of Newton iterations for finding the OT mesh
        label maxMAiters() const { return maxMAiters_; }
        
        //- The maximum allowable mesh velocity
        scalar maxMeshVelocity() const { return maxMeshVelocity_; }
        
        //- The proportion of the mesh movement actually applied
        scalar meshRelax() const { return meshRelax_; }

        //- Smoothing coefficient of the monitor function
        scalar smoothCoeff() const { return smoothCoeff_;}
        
        //- The mesh potential
        const volScalarField& meshPot() const { return meshPot_;}

        //- name of the vector field to use to calculate the monitor function
        const word monitorFromName() const { return monitorFromName_;}

        //- Set the monitor function from a field on the physical mesh
        void setMonitor();

        //- Update the mesh for both mesh motion (no topology change)
        virtual bool update();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
