// Make the flux relative
fvc::makeRelative(phi, U);

for(int icorr = 0; icorr < nIteration; icorr++)
{
    fvScalarMatrix TEqn
    (
        fvm::ddt(T)
      + fvc::div(phi, T)
    );
    TEqn.solve();

    fvScalarMatrix uniTEqn
    (
        fvm::ddt(uniT)
      + fvc::div(phi, uniT, "div(phi,T)")
    );
    uniTEqn.solve();
}

// Recover the absolute flux
fvc::makeAbsolute(phi, U);
