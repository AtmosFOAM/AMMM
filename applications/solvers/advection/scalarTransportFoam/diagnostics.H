{
    scalar AVT = fvc::domainIntegrate(A*T).value();
    scalar Vtot = gSum(mesh.V());
    scalar normalisedVolumeChange = Vtot/Vtot0;
    scalar globalSum = AVT/Vtot;
    scalar normalisedMassChange = (AVT - AVT0)/AVT0;
    os << runTime.timeName() << "    "
       << normalisedVolumeChange << "    " << AVT << "    "
       << globalSum << "    "
       << normalisedMassChange << "    "
       << endl;
}
