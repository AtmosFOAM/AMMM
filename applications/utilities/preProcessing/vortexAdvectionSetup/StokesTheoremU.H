const scalar Htop = U.mesh().bounds().span().z();
//Info << "H = " << H << endl;
const fvMesh& pmesh = U.mesh();
forAll(U,faceI)
{
    // Circulate around the vertices of the face and sum to contribution
    // to the flux
    const face& f = pmesh.faces()[faceI];
    //const scalar& h = rh0Faces[faceI];
    //const scalar& H = mesh().bounds().span().z();
    point p0 = pmesh.points()[f.last()];
    point p1 = pmesh.points()[f.first()];
    point pmid = 0.5*(p0 + p1);
    scalar radius1 = p1.y();
    scalar radius0 = p0.y();
    U[faceI] = streamFunctionVortexAdvection(pmid,u0.value(),Htop,orog_height,orog_start,orog_stop,dx)
                *(radius1 - radius0);
    for(label ip = 1; ip < f.size(); ip++)
    {
        //Info << "ip = " << ip << " f.size = " << f.size() << endl;
        p0 = p1;
        p1 = pmesh.points()[f[ip]];
        point pmid = 0.5*(p0 + p1);
        scalar radius1 = p1.y();
        scalar radius0 = p0.y();
        U[faceI] += streamFunctionVortexAdvection(pmid,u0.value(),Htop,orog_height,orog_start,orog_stop,dx)
                    *(radius1-radius0);
    }
}
