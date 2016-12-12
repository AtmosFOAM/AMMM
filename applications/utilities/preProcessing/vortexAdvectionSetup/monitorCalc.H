// calculates the monitor function value on the pMesh

q*= 0.15/max(mag(q)).value();
monitorP = Foam::sqrt( 1.0 + m1*(q & q) );
for (int smooth=0; smooth<nSmooth; smooth++)
    {
        //Info << "Smoothing iteration " << smooth << " of " << nSmooth << endl;
        //Info << "min mon = " << min(monitorP) << " max mon = " << max(monitorP) << endl;
        monitorP += m2*fvc::laplacian(monitorP);
  
    }


//Info << "min mon = " << min(monitorP) << " max mon = " << max(monitorP) << endl;
