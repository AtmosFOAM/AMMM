pointField meshPointsMovement = meshUpoints-pMesh.points();
pointField meshPointsOld = pMesh.points();
bool moveTooLarge = true;
scalar moveFactor = 1.0;
int moveScal = 0;
while (moveTooLarge)
{
    pMesh.movePoints(meshPointsOld + moveFactor*meshPointsMovement);
    meshPhi = fvc::meshPhi(u);
    phi -= meshPhi;
    #include "CourantNo.H"
    Info << " moveScal = " << moveScal << " moveFactor = " << moveFactor
         << " max meshPhi = " << max(meshPhi) << " CoNum = " << CoNum << endl;
    if ( CoNum > 0.3 )
    {
        moveFactor *= 0.5;
    }
    else
    {
        moveTooLarge = false;
        meshUpoints = meshPointsOld + moveFactor*meshPointsMovement;
    }
    phi += meshPhi;
    pMesh.movePoints(meshPointsOld);
    moveScal++;
}

