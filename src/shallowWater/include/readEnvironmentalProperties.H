Info << "\nReading environmentalProperties" << endl;

IOdictionary environmentalProperties
(
    IOobject
    (
        "environmentalProperties",
        runTime.constant(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    )
);

const dimensionedScalar g(environmentalProperties.lookup("g"));
const Switch rotating(environmentalProperties.lookup("rotating"));
Info<< "about to define beta in readenvironmentalProperties.H \n" << endl;
const dimensionedScalar beta = rotating ?
                    environmentalProperties.lookup("beta") :
                    dimensionedScalar("beta", dimless/(dimTime*dimLength), scalar(0));
Info<< "finished reading environmental properties \n" << endl;
