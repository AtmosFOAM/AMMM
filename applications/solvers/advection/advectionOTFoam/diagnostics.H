{
    scalar Vn = gSum(mesh.V());
    scalar ATn = fvc::domainIntegrate(A*T).value();
    scalar Tn = fvc::domainIntegrate(T).value();
    scalar normalisedVolChange = (Vn - V0)/V0;
    scalar normalisedATChange = (ATn - AT0)/AT0;
    scalar normalisedTChange = (Tn - T0)/T0;

    os << runTime.timeName() << "    "
       << normalisedVolChange << "    " << normalisedATChange << "    "
       << normalisedTChange << endl;
}
