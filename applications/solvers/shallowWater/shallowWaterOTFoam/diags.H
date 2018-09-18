{
    // Potential and kinetic energies
    volScalarField energyField
    (
        "energyField",
        g*h*(0.5*h+h0) + 0.5*h*magSqr(U)
    );
    
    energy = fvc::domainIntegrate(energyField).value()/Vtot;
    momentum = fvc::domainIntegrate(h*U).value()/Vtot;
    energyDiff = (energy - energy0)/energy0;
    momentumDiff = (momentum - momentum0)/mag(momentum0);
}
    os << runTime.timeName() << " " << energy << " " << energyDiff << " "
       << momentum << " " << momentumDiff << endl;

