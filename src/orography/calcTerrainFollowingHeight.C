#include "fvCFD.H"
#include "calcTerrainFollowingHeight.H"
#include "evalOrography.H"

point calcTerrainFollowingHeight(point& p, scalar& ztop){
    
    const scalar z = p.z();

    point terrainFollowingPoint = p;

    const scalar h = evalOrography(p);
    
    terrainFollowingPoint.z() = z + ( 1 - (z/ztop) )*h;

    return terrainFollowingPoint;
}





