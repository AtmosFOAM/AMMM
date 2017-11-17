// First move the physical (pMesh) point to the rMesh then raise the orography
pointField newPPointsF = mesh.points();

forAll(newPPointsF, ip)
{
    newPPointsF[ip] = transform->physicalToComputational(newPPointsF[ip]);
}

mesh.movePoints(newPPointsF);
