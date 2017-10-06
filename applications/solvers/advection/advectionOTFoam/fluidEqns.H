// Calculate absolute flux from the mapped surface velocity
phi = hUf & mesh.Sf();

// Make the flux relative to the mesh motion
fvc::makeRelative(phi, h, U);

// Momentum predictor
fvVectorMatrix UEqn
(
    fvm::ddt(h,U)
  + fvc::div(phi, U)
  + h*(twoOmega ^ U)
);

// fvc::makeRelative(phi, U);

// for(int icorr = 0; icorr < nIteration; icorr++)
// {
//     fvScalarMatrix TEqn
//     (
//         fvm::ddt(T)
//       + fvc::div(phi, T)
//     );
//     TEqn.solve();
// }
