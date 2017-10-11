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

divU = fvc::div(phi);
divU.write();

fvc::makeAbsolute(phi, U);
