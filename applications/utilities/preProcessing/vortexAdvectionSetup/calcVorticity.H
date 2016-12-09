meshUpoints = rMesh.points();
pMesh.movePoints(meshUpoints);

Uf *= 0.0;
 
forAll(Uf,face){
    scalar x = (Uf.mesh().faceCentres()[face].x()-vortexCentre.x());
    scalar y = (Uf.mesh().faceCentres()[face].y()-vortexCentre.y());
    scalar z = (Uf.mesh().faceCentres()[face].z()-vortexCentre.z());
        
    vector direction = vector(z,0,-x);
    direction /=mag(direction);
    scalar vortexDistance = mag(vector(x,y,z));
    Uf[face] = direction * vortexMagnitude*Foam::exp(-sqr(vortexDistance)/(2*sqr(vortexRadius)));//*vortexDistance);//* Foam::exp(-(  sqr(vortexDistance-vortexRadius) ));

}

forAll(Uf,face){
    scalar x = (Uf.mesh().faceCentres()[face].x()-vortexCentre.x());
    scalar y = (Uf.mesh().faceCentres()[face].y()-vortexCentre.y());
    scalar z = (Uf.mesh().faceCentres()[face].z()-(vortexCentre.z()+3*vortexRadius));
        
    vector direction = vector(z,0,-x);
    direction /=mag(direction);
    scalar vortexDistance = mag(vector(x,y,z));
    Uf[face] -= direction * vortexMagnitude*Foam::exp(-sqr(vortexDistance)/(2*sqr(vortexRadius)));//*vortexDistance);//* Foam::exp(-(  sqr(vortexDistance-vortexRadius) ));

}

        

u *= 0.0;
 
forAll(u,celli){
    scalar x = (u.mesh().C()[celli].x()-vortexCentre.x());
    scalar y = (u.mesh().C()[celli].y()-vortexCentre.y());
    scalar z = (u.mesh().C()[celli].z()-vortexCentre.z());
        
    vector direction = vector(z,0,-x);
    direction /=mag(direction);
    scalar vortexDistance = mag(vector(x,y,z));
    u[celli] = direction * vortexMagnitude*Foam::exp(-sqr(vortexDistance)/(2*sqr(vortexRadius)));//*vortexDistance);//* Foam::exp(-(  sqr(vortexDistance-vortexRadius) ));
}

forAll(u,celli){
    scalar x = (u.mesh().C()[celli].x()-vortexCentre.x());
    scalar y = (u.mesh().C()[celli].y()-vortexCentre.y());
    scalar z = (u.mesh().C()[celli].z()-(vortexCentre.z()+3*vortexRadius));
        
    vector direction = vector(z,0,-x);
    direction /=mag(direction);
    scalar vortexDistance = mag(vector(x,y,z));
    u[celli] -= direction * vortexMagnitude*Foam::exp(-sqr(vortexDistance)/(2*sqr(vortexRadius)));//*vortexDistance);//* Foam::exp(-(  sqr(vortexDistance-vortexRadius) ));
}


    { HodgeOps H(pMesh);
        u = H.reconstruct(Uf & pMesh.Sf());
        q = fvc::curl(u);
    }
    
q = fvc::curl(u);
q.write();
