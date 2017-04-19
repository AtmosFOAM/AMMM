/*---------------------------------------------------------------------------*\
Description
    Calculates the blending coefficient between the A and C-grid for 
    reconstructing the velocity (calculates from the pressure)

\*---------------------------------------------------------------------------*/

#ifndef ACblend_H
#define ACblend_H

namespace Foam
{

tmp<volScalarField> calcACblend(const volScalarField& p)
{
    const fvMesh& mesh = p.mesh();

    tmp<volScalarField> tb
    (
        new volScalarField
        (
            IOobject("ACblend", p.instance(), mesh),
            mesh,
            dimensionedScalar("ACblend", dimless, scalar(0))
        )
    );
    volScalarField& b = tb.ref();
    
    surfaceScalarField gradpf = fvc::snGrad(p);
    volVectorField gradp = fvc::grad(p);

    // Add contribution for b for every face in the mesh
    forAll(gradpf, faceI)
    {
        const vector n = mesh.Sf()[faceI]/mesh.magSf()[faceI];
        const label own = mesh.owner()[faceI];
        const label nei = mesh.neighbour()[faceI];
        const scalar gf = gradpf[faceI];
        const scalar go = gradp[own]&n;
        const scalar gn = gradp[nei]&n;

        // Not enough high values and too spiky
        //scalar maxGradSqr = max
        //(
        //    max(sqr(gf), sqr(go)), max(sqr(gn), VSMALL)
        //);
        // scalar bf = (gf - go)*(gf - gn)/maxGradSqr;

        // Too many high values - del^4. -> b>0.1 almost everywhere
        scalar bf = mag(2*gf - go - gn)/(mag(go) + mag(gn) + VSMALL);

        // Too noisy
        //scalar bf = (gf - go)*(gf - gn)/(sqr(go) + sqr(gn) + VSMALL);

        bf = min(max(bf, scalar(0)), scalar(1));
        
        // Just picks up loads of zero gradients
        //scalar bf = 0;
        //if (sign(gf - go) == sign(gf - gn)) bf = 0.5;
        
        b[own] += bf;
        b[nei] += bf;
    }
    Info << "ACblend goes from " << min(b).value() << " to " << max(b).value()
         << endl;
    b = min(b, scalar(1));
    
    return tb;
}

}
#endif

// ************************************************************************* //

