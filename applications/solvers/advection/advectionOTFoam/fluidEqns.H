// Make the flux relative
fvc::makeRelative(phi, U);

flowOverGround.ref().field() = phi.boundaryField()[5];

if (colinParameter)
{
    // Reset A
    if (resetA)
    {
        mesh.V0() == A.oldTime()*mesh.V0();
	T.oldTime() == A.oldTime()*T.oldTime();
        A.oldTime() == dimensionedScalar("", dimless, scalar(1));
    }

    fvScalarMatrix AEqn
    (
        fvm::ddt(A)
      - fvc::div(mesh.phi())
    );
    AEqn.solve();

    // Solve equations iterativelly
    for(int icorr = 0; icorr < nIteration; icorr++)

      {
        fvScalarMatrix AuniTEqn
	(
	    fvm::ddt(A, uniT)
          + fvc::div(phi, uniT, "div(phi,T)")
        );
	AuniTEqn.solve();

	fvScalarMatrix ATEqn
	(
	    fvm::ddt(A, T)
          + fvc::div(phi, T, "div(phi,T)")
        );
	ATEqn.solve();
    }
    
    // // Reset A to unity
    // if (resetA)
    // {
    //     mesh.V0() == A.oldTime()*mesh.V0();
    // 	A.oldTime() == dimensionedScalar("", dimless, scalar(1));
    // 	AuniT.oldTime() == uniT.oldTime();
    // 	//	AT.oldTime() == T.oldTime();
    // }
    // // Solve equations iterativelly
    // for(int icorr = 0; icorr < nIteration; icorr++)
    // {
    //     fvScalarMatrix AEqn
    //     (
    // 	    fvm::ddt(A)
    // 	  + fvc::div(phi, A, "div(phi,T)")
    //     );
    //     AEqn.solve();

    //     fvScalarMatrix ATEqn
    //     (
    //         fvm::ddt(AT)
    //       + fvc::div(phi, AT, "div(phi,T)")
    //     );
    //     ATEqn.solve();

    //     fvScalarMatrix AuniTEqn
    //     (
    //         fvm::ddt(AuniT)
    //       + fvc::div(phi, AuniT, "div(phi,T)")
    //     );
    //     AuniTEqn.solve();
    // }

    // // Update T and uniT
    // T     == AT/A;
    // uniT  == AuniT/A;
}
else
{
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
}

// Update AT
AT == A*T;

// Recover the absolute flux
fvc::makeAbsolute(phi, U);
