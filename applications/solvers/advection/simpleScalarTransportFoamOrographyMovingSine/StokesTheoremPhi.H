const scalar H = phi.mesh().bounds().span().z();
//Info << "H = " << H << endl;
const fvMesh& pmesh = phi.mesh();
forAll(phi,faceI)
{
    // Circulate around the vertices of the face and sum to contribution
    // to the flux
    const face& f = pmesh.faces()[faceI];
    const scalar& h = rh0Faces[faceI];
    //const scalar& H = mesh().bounds().span().z();
    point p0 = pmesh.points()[f.last()];
    point p1 = pmesh.points()[f.first()];
    point pmid = 0.5*(p0 + p1);
    //Info << " pmid = " << pmid << endl;
    //Info << "inputs to sta: " << pmid << " " << u0.value() << " " << H << " " << h << endl;
    phi[faceI] = streamFunctionAt(pmid,u0.value(),H,h)*(p1.y() - p0.y());
    for(label ip = 1; ip < f.size(); ip++)
        {
            //Info << "ip = " << ip << " f.size = " << f.size() << endl;
            p0 = p1;
            p1 = pmesh.points()[f[ip]];
            point pmid = 0.5*(p0 + p1);
            //Info << "inputs to sta: pmid =  " << pmid << endl;
            //Info << "inputs to sta: u0.value = " << u0.value() << endl;
            //Info << "inputs to sta: H = " << H << endl;
            //Info << "inputs to sta: h = " << h << endl;
            phi[faceI] += streamFunctionAt(pmid,u0.value(),H,h)*(p1.y() - p0.y());
            //Info << "ended loop " << endl;
        }
}
