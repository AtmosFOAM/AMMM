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

#include "monitorFunctionFront.H"
#include "addToRunTimeSelectionTable.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "VectorSpaceFunctions.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(monitorFunctionFront, 0);
addToRunTimeSelectionTable(monitorFunction, monitorFunctionFront, dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

monitorFunctionFront::monitorFunctionFront
(
    const IOdictionary& dict
)
:
    monitorFunction(dict),
    p0_(readScalar(dict.lookup("p0"))),
    m_(readScalar(dict.lookup("m"))),
    resolution_(readScalar(dict.lookup("resolution"))),
    width_(readScalar(dict.lookup("width"))),
    alpha_(readScalar(dict.lookup("alpha"))),
    gamma_(readScalar(dict.lookup("gamma"))),
    meshperiod_(readScalar(dict.lookup("meshperiod")))
{}

// * * * * * * * * * * * * * Member Functions * * * * * * * * * * * * * * //

//tmp<scalarField> monitorFunctionFront::map
//(
//    const pointField& pts,
//    const scalarField& oldMonitor
//) const
//{
//    tmp<scalarField> tMon(new scalarField(pts.size(), scalar(0)));
//    scalarField& mon = tMon();

//    point centreHat = spherical_? point(unitVector(centre_)) : centre_;
//    
//    forAll(mon, ip)
//    {
//        scalar r = spherical_? acos
//                  (
//                    point(unitVector(pts[ip])) & centreHat
//                  )
//                 : mag(pts[ip] - centre_);
//        mon[ip] = sqrt(0.5/(1+gamma_)*(tanh((beta_-r)/alpha_)+1) + gamma_);
//    }
//    
//    return tMon;
//}


tmp<volScalarField> monitorFunctionFront::map
(
    const fvMesh& newMesh,
    const volScalarField& oldMonitor
) const
{
    tmp<volScalarField> tMon
    (
        new volScalarField
        (
            IOobject("monitor", newMesh.time().timeName(), newMesh,
                     IOobject::NO_READ, IOobject::AUTO_WRITE),
            newMesh,
            dimensionSet(0,0,0,0,0),
            wordList(newMesh.boundaryMesh().size(), "zeroGradient")
        )
    );
    volScalarField& mon = tMon.ref();
    
    scalar t = newMesh.time().value()/meshperiod_;
    
    forAll(mon, cellI)
    {
        scalar x = newMesh.C()[cellI].x();
        scalar z = newMesh.C()[cellI].z();
        scalar px = p0_ + m_*z + t*(alpha_ + gamma_*z*z);
        //scalar dis = std::min(std::abs(px - x), dx - std::abs(px -x) );
        scalar dis = std::abs(px-x);
        mon[cellI] = 1.0 + resolution_*std::exp( -dis*dis/width_ );
    }
    
    mon.correctBoundaryConditions();
    return tMon;
}


tmp<surfaceVectorField> monitorFunctionFront::grad
(
    const fvMesh& newMesh,
    const surfaceVectorField& oldMonitor
) const
{
    // Why is this function empty?
    NotImplemented;
    return surfaceVectorField::null();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //



} // End namespace Foam

// ************************************************************************* //
