{
    scalar AVT = fvc::domainIntegrate(A*T).value();
    scalar Vtot = gSum(mesh.V());
    scalar normalisedVolumeChange = (Vtot-Vtot0)/Vtot0;
    scalar normalisedMassChange = (AVT - AVT0)/AVT0;
    os << runTime.timeName() << "    "
       << normalisedVolumeChange << "    " << AVT << "    "
       << normalisedMassChange << "    "
       << endl;
}
