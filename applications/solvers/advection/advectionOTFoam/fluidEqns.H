fvc::makeRelative(phi, U);

for(int icorr = 0; icorr < nIteration; icorr++)
{
    fvScalarMatrix TEqn
    (
        fvm::ddt(T)
      + fvc::div(phi, T)
    );
    TEqn.solve();
}

fvc::makeAbsolute(phi, U);
