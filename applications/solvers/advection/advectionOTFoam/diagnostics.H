{
    scalar Vn = gSum(mesh.V());
    scalar AVn = fvc::domainIntegrate(A).value();
    scalar Tn = fvc::domainIntegrate(T).value();
    scalar ATn = fvc::domainIntegrate(A*T).value();

    scalar normalisedVChange = (Vn - V0)/V0;
    scalar normalisedAVChange = (AVn - AV0)/AV0;
    scalar normalisedTChange = (Tn - T0)/T0;
    scalar normalisedATChange = (ATn - AT0)/AT0;
    
    os << runTime.timeName() << "    "
       << Vn << "    " << AVn << "    "
       << normalisedVChange << "    "
       << normalisedAVChange << "    "
       << normalisedTChange << "    "
       << normalisedATChange << "    " 
       << endl;
}
