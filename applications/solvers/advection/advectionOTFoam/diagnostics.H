{
    scalar Vn = gSum(mesh.V());
    scalar AVn = fvc::domainIntegrate(A).value();
    scalar Tn = fvc::domainIntegrate(T).value();
    scalar ATn = fvc::domainIntegrate(A*T).value();
    scalar uniTn = fvc::domainIntegrate(uniT).value();
    scalar AuniTn = fvc::domainIntegrate(A*uniT).value();

    scalar VChange = (Vn - V0)/V0;
    scalar AVChange = (AVn - AV0)/AV0;
    scalar TChange = (Tn - T0)/T0;
    scalar ATChange = (ATn - AT0)/AT0;
    scalar uniTChange = (uniTn - uniT0)/uniT0;
    scalar AuniTChange = (AuniTn - AuniT0)/AuniT0;

    os << runTime.timeName() << "    "
       << Vn << "    "
       << AVn << "    "
       << VChange << "    "
       << AVChange << "    "
       << TChange << "    "
       << ATChange << "    "
       << uniTChange << "    "
       << AuniTChange << "    "
       << endl;
}
