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
    //surfaceVectorField delta = mesh.delta();
    //surfaceScalarField dist = 1/mesh.deltaCoeffs();

    // Add contribution for b for every face in the mesh
    forAll(gradpf, faceI)
    {
        const vector n = mesh.Sf()[faceI]/mesh.magSf()[faceI];
        const label own = mesh.owner()[faceI];
        const label nei = mesh.neighbour()[faceI];
        //const vector d = delta[faceI];
        //const scalar dx = dist[faceI];
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
        //scalar bf = mag(2*gf - go - gn)/(mag(go) + mag(gn) + VSMALL);
        //scalar bf = mag(2*gf - go - gn)
        //           /(2*mag(gf) + mag(go) + mag(gn) + VSMALL);

        // Too noisy - try smoothing it
        scalar bf = (gf - go)*(gf - gn)/(sqr(go) + sqr(gn) + VSMALL);

        //bf = min(max(bf, scalar(0)), scalar(1));
        //bf = max(bf, scalar(0));
        
        // Just picks up loads of zero gradients
        //scalar bf = 0;
        //if (sign(gf - go) == sign(gf - gn)) bf = 0.5;
        
        b[own] += bf;
        b[nei] += bf;
    }
    Info << "Before smoothing ACblend goes from " << min(b).value() << " to " 
         << max(b).value() << endl;
         
    b == max(min(b, scalar(0.1)), scalar(0));
    b.correctBoundaryConditions();

    for(int iSmooth = 0; iSmooth < 10; iSmooth++)
    {
        b += 0.2*fvc::laplacian(1/sqr(mesh.deltaCoeffs()), b, "laplacian(b)");
    }
    Info << "After smoothing and limiting ACblend goes from " << min(b).value()
         << " to "  << max(b).value() << endl;
    
    return tb;
}

}
#endif

// ************************************************************************* //

