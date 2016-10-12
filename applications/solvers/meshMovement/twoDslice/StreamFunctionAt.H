scalar streamFunctionAt(const point& p, const scalar& u0, const scalar& H, const scalar& orog_height, const scalar& orog_start, const scalar& orog_stop, const scalar& dx)
{
    // Info << "hello from streamFunctionAT" << endl;
    // Info << "streamFunctionAT p = " << p << endl;
    // Info << "streamFunctionAT u0 = " << u0 << endl;
    // Info << "streamFunctionAT H = " << H << endl;
    // Info << "streamFunctionAT h = " << h << " p.z() = " << p.z() << " s = " << -u0 * (H * (p.z() ) / (H - h)) << " p.z() - h = " << p.z() - h << endl;
    //const scalar pi = Foam::constant::mathematical::pi;
    scalar h = 0;
    scalar theta = p.x()/dx;
    if (theta > orog_start and theta < orog_stop) {
        h = orog_height;
    }
    //Info << " h = " << h << endl;
    if (p.z() > H)
        {
            return -u0 * p.z();
        }
    else
        {
            return -u0 * (H * (p.z() - h) / (H - h));
            //return -u0 * (H * (p.z() - h) / (H - h));
        }
};
