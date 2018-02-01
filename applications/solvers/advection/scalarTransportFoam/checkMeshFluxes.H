volScalarField volOld = volRatio;
volOld.field() *= pMesh.V0();
volScalarField volNew = volRatio;
volNew.field() *= pMesh.V();
volScalarField volError
(
    "volError",
   volRatio - 1 + dt*fvc::div(pMesh.phi())
);
volError.write();
FatalErrorIn("") << exit(FatalError);

