// takes monitorP on the pMesh and evaluates the corresponding monitorM on the (computational) mesh

//in this case, I am assuming no orography, so I just copy the field
setInternalAndBoundaryValues(monitorM, monitorP);
setInternalAndBoundaryValues(monitorR, monitorP);

