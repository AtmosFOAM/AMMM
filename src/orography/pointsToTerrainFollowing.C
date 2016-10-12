#include "fvCFD.H"
#include "calcTerrainFollowingHeight.H"
#include "pointsToTerrainFollowing.H"

int pointsToTerrainFollowing(pointField& points, scalar& ztop){
    forAll(points,p){
        points[p] = calcTerrainFollowingHeight(points[p],ztop);
    }
    return 0;
}






