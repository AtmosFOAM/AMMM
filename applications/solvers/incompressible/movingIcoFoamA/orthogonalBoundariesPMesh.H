//
// Make the boundaries of the fvMesh orthogonal (by moving boundary face centres)
// ~~~~~~~~~~~~

    Foam::Info << "Making the boundary faces of pMesh orthogonal" << Foam::endl;

    forAll(pMesh.boundary(), patchi)
    {
        const fvPatch& pat = pMesh.boundary()[patchi];
        // only move centres of un-coupled boundary faces
        if (!pat.coupled())
        {
            // loop through and move all of the faces
            for(label facei = pat.start(); facei <pat.start()+pat.size();facei++)
            {
                label own = pMesh.faceOwner()[facei];
                const vector& C = pMesh.C()[own];
                const vector& Cf = pMesh.faceCentres()[facei];
                const vector Sfhat = pMesh.faceAreas()[facei]
                                      /mag(pMesh.faceAreas()[facei]);
                const vector fCnew = C + ((Cf - C) & Sfhat)*Sfhat;

                const_cast<vectorField&>(pMesh.faceCentres())[facei] = fCnew;
            }
        }
    }
