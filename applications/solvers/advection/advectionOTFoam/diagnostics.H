{
    scalar Vn = gSum(mesh.V());
    scalar ATn = fvc::domainIntegrate(A*T).value();
    scalar Tn = fvc::domainIntegrate(T).value();
    scalar globalSum = ATn/Vn;
    scalar normalisedVolChange = (Vn - V0)/V0;
    scalar normalisedATChange = (ATn - AT0)/AT0;
    scalar normalisedTChange = (Tn - T0)/T0;

    os << runTime.timeName() << "    "
       << Vn << "    " << ATn << "    " << Tn << "    "
       << globalSum << "    "
       << endl;

    os_vol << normalisedVolChange << "    "
           << endl;

    os_AT << normalisedATChange << "    "
          << endl;

    os_T << normalisedTChange << "    "
         << endl;
}
