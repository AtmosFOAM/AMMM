// First move the physical (pMesh) point to the rMesh then raise the orography
pointField newPPoints = rMesh.points();

forAll(newPPoints, ip)
{
    newPPoints[ip] = transform->computationalToPhysical(newPPoints[ip]);
}

pMesh.movePoints(newPPoints);
