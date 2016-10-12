scalar dx = mesh.bounds().span().x();
const bool fixed_lid=true;
// forAll(meshUpoints,point)
// {
//     if(meshUpoints[point].z() < 0.99 ) //i.e. not on the outer shell
//     { 
//         scalar x = meshUpoints[point].x()/dx;
//         if (x > orog_start and x < orog_stop)
//         {
//             //set the new height
//             meshUpoints[point].z() = orog_height;
//         }
//         else
//         {
//             meshUpoints[point].z() = 0;
//         }
//     } else if( ! fixed_lid )
//         { 
//         scalar x = meshUpoints[point].x()/dx;
//         if (x > orog_start and x < orog_stop)
//         {
//             //set the new height
//             meshUpoints[point].z() = 1.0+orog_height;
//         }
//         else
//         {
//             meshUpoints[point].z() = 1.0;
//         }
//     }
// }


label PatchId = mesh.boundaryMesh().findPatchID("z0");
const polyPatch& namePolyPatch = mesh.boundaryMesh()[PatchId];
//Info << PatchId << endl;
//Info << namePolyPatch << endl;
//Info << namePolyPatch.meshPoints() << endl;
forAll(namePolyPatch.meshPoints(),point){
    scalar x = meshUpoints[point].x()/dx;
    if (x > orog_start and x < orog_stop)
        {
            //set the new height
            meshUpoints[point].z() = orog_height;
        }
    else
        {
            meshUpoints[point].z() = 0;
        }
}
