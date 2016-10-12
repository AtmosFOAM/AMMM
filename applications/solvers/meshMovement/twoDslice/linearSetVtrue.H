forAll(Vtrue,c){
    Vtrue[c] = pMesh.V()[c];
    const labelList& cellP = pMesh.cellPoints()[c];
    scalar xmin=999999999999;
    scalar xmax=-xmin;
    forAll(cellP,point){
        //Info << point << " " << pMesh.points()[cellP[point]].x() << endl;
        xmin = min(xmin,pMesh.points()[cellP[point]].x());
        xmax = max(xmax,pMesh.points()[cellP[point]].x());
    }

    if( xmax < dx*orog_start ) //case 1: cell is left of orography:
        {
            //Info << " case 1 " << c << endl;
            Vtrue[c] = dy*dz*(xmax-xmin);
        } else if (xmin > dx*orog_stop ) //case 2: cell is right or orography:  
        {
            //Info << " case 2 " << c << endl;
            Vtrue[c] = dy*dz*(xmax-xmin);
        } else if (xmin < dx*orog_start && xmax < dx*orog_stop) //case 3: cell overlaps the left edge of the orography
        {
            //Info << " case 3 " << endl;
            Vtrue[c] = dy*((dx*orog_start-xmin)*dz+(xmax-dx*orog_start)*(dz-orog_height));
        } else if (xmin > dx*orog_start && xmax > dx*orog_stop) //case 4: cell overlas the right edge of the orography
        {
            //Info << " case 4 " << endl;
            Vtrue[c] = dy*((xmax-dx*orog_stop)*dz+(dx*orog_stop-xmin)*(dz-orog_height));        
        } else //case 5: cell is completely within the orography
        {
            //Info << " case 5 " << endl;
            Vtrue[c] = dy*(dz-orog_height)*(xmax-xmin);
        }
    
    //Info << cellP << " " << xmin << " " << xmax << endl;
    //Info << "V = " << V[c] << " Vtrue = " << Vtrue[c] << endl; 
}
