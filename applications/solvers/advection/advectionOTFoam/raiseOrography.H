// First move the physical (pMesh) point to the rMesh then raise the orography
pointField newPPoints = mesh.points();

forAll(newPPoints, ip)
{
    newPPoints[ip] = transform->computationalToPhysical(newPPoints[ip]);
}

mesh.movePoints(newPPoints);
