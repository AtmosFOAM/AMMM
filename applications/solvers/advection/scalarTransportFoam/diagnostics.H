{
    scalar AVT = fvc::domainIntegrate(A*T).value();
    scalar normalisedMassChange = (AVT - AVT0)/AVT0;
    os << runTime.timeName() << "    " << normalisedMassChange << endl;
}
