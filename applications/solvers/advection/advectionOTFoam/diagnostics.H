{
    scalar VT = fvc::domainIntegrate(T).value();
    scalar Vtot = gSum(mesh.V());
    scalar normalisedVolumeChange = Vtot/Vtot0;
    scalar globalSum = VT/Vtot;
    scalar normalisedMassChange = (VT - VT0)/VT0;
    os << runTime.timeName() << "    "
       << normalisedVolumeChange << "    " << VT << "    "
       << globalSum << "    "
       << normalisedMassChange << "    "
       << endl;
}
