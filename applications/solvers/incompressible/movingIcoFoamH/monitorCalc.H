// calculates the monitor function value on the pMesh

monitorP = Foam::sqrt( 1.0 + m1*(q & q) );
monitorP += m2*fvc::laplacian(monitorP);
monitorP += 1.0 - min(monitorP);
Info << "min mon = " << min(monitorP) << " max mon = " << max(monitorP) << endl;
