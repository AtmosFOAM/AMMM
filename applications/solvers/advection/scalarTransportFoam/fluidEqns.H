// Fixed number of iterations per time-step
for (int corr = 0; corr < nRKstages; corr++)
{
    // Make the flux relative
    fvc::makeRelative(phi, U);
    if (corr == 0)
    {
        #include "CourantNo.H"
    }

    fvScalarMatrix TEqn
    (
        fvm::ddt(T)
      + fvc::div(phi, T)
    );
    TEqn.solve();

//    // Time-stepping by hand to check both approaches
//    divPhiT = fvc::div(phi, T)*mesh.V();
//    T = 
//    (
//        T.oldTime().mesh.V0() - dt*
//        (
//            (1-offCentre)*divPhiT.oldTime()
//          + offCentre*divPhiT
//        )
//    )/mesh.V();
}


// Gradient of T for the monitor function
gradT = fvc::interpolate(fvc::grad(T, "gradT"), "gradT");
