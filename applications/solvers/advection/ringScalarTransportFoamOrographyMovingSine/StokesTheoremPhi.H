const scalar H = phi.mesh().bounds().span().z();
//Info << "H = " << H << endl;
const fvMesh& pmesh = phi.mesh();
forAll(phi,faceI)
{
    // Circulate around the vertices of the face and sum to contribution
    // to the flux
    const face& f = pmesh.faces()[faceI];
    //const scalar& h = rh0Faces[faceI];
    //const scalar& H = mesh().bounds().span().z();
    point p0 = pmesh.points()[f.last()];
    point p1 = pmesh.points()[f.first()];
    point pmid = 0.5*(p0 + p1);
    //Info << " pmid = " << pmid << endl;
    //Info << "inputs to sta: " << pmid << " " << u0.value() << " " << H << " " << h << endl;
    scalar radius1 = Foam::sqrt( sqr(p1.x()) + sqr(p1.y()) );
    scalar radius0 = Foam::sqrt( sqr(p0.x()) + sqr(p0.y()) );
    phi[faceI] = streamFunctionAt(pmid,u0.value(),H,orog_height)*(radius1 - radius0);
    for(label ip = 1; ip < f.size(); ip++)
        {
            //Info << "ip = " << ip << " f.size = " << f.size() << endl;
            p0 = p1;
            p1 = pmesh.points()[f[ip]];
            point pmid = 0.5*(p0 + p1);
            scalar radius1 = Foam::sqrt( sqr(p1.x()) + sqr(p1.y()) );
            scalar radius0 = Foam::sqrt( sqr(p0.x()) + sqr(p0.y()) );
            // Info << "inputs to sta: pmid =  " << pmid << endl;
            // Info << "inputs to sta: u0.value = " << u0.value() << endl;
            // Info << "inputs to sta: H = " << H << endl;
            // Info << "inputs to sta: h = " << h << endl;
            phi[faceI] += streamFunctionAt(pmid,u0.value(),H,orog_height)*(radius1-radius0);
            //Info << "ended loop " << endl;
        }
    //Info << "phi = " << phi[faceI] << " mesh.sf = " << rMesh.Sf()[faceI] << endl;
}
