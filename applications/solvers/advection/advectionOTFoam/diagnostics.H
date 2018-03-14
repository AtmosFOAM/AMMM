{
    scalar Vn = gSum(mesh.V());
    scalar AVn = fvc::domainIntegrate(A).value();
    scalar Tn = fvc::domainIntegrate(T).value();
    scalar ATn = fvc::domainIntegrate(A*T).value();

    scalar VChange = (Vn - V0)/V0;
    scalar AVChange = (AVn - AV0)/AV0;
    scalar TChange = (Tn - T0)/T0;
    scalar ATChange = (ATn - AT0)/AT0;

    os << runTime.timeName() << "    "
       << Vn << "    "
       << AVn << "    "
       << VChange << "    "
       << AVChange << "    "
       << TChange << "    "
       << ATChange << "    "
       << endl;
}
