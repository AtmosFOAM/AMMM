forAll(meshUpoints,point)
{
    if(meshUpoints[point].z() < 0.99 ) //i.e. not on the outer shell
    { 
        scalar x = meshUpoints[point].x();
        scalar y = meshUpoints[point].y();
        scalar theta = Foam::atan2(y,x);
        if (theta > pi/2 and theta < 9*pi/16)
        {
            //set the new height
            meshUpoints[point].z() = orog_height;
        }
        else
        {
            meshUpoints[point].z() = 0;
        }
    }
}
