#include "fvCFD.H"
#include "setInternalValues.H"

int patchShell(fvMesh& mesh, scalar inner_radius, scalar outer_radius){

    pointField targetPoints = mesh.points();
        
    const polyPatchList& patches = mesh.boundaryMesh();
    forAll(patches, patchi)
        {
            const polyPatch& patch = patches[patchi];
            if(patch.name() == "outerShell")
                {
                    const labelList& meshPoints = patch.meshPoints();
                    forAll(meshPoints, ip)
                        {
                            point& meshPoint = targetPoints[meshPoints[ip]];
                            scalar x = meshPoint.x();
                            scalar y = meshPoint.y();
                            scalar r = Foam::sqrt(sqr(x) + sqr(y));
                            meshPoint.x() = outer_radius*x/r;
                            meshPoint.y() = outer_radius*y/r;
                        }
                }
            else if(patches[patchi].name() == "innerShell")
                {
                    const labelList& meshPoints = patch.meshPoints();
                    forAll(meshPoints, ip){
                        point& meshPoint = targetPoints[meshPoints[ip]];
                        scalar x = meshPoint.x();
                        scalar y = meshPoint.y();
                        scalar r = Foam::sqrt(sqr(x) + sqr(y));
                        meshPoint.x() = inner_radius*x/r;
                        meshPoint.y() = inner_radius*y/r;
                    }
                }
        }
    
    mesh.movePoints(targetPoints);

    return 0;
}
