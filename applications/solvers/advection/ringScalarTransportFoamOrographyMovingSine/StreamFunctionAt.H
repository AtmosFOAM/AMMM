scalar streamFunctionAt(const point& p, const scalar& u0, const scalar& H, const scalar& orog_height)
{
    // Info << "hello from streamFunctionAT" << endl;
    // Info << "streamFunctionAT p = " << p << endl;
    // Info << "streamFunctionAT u0 = " << u0 << endl;
    // Info << "streamFunctionAT H = " << H << endl;
    // Info << "streamFunctionAT h = " << h << " p.z() = " << p.z() << " s = " << -u0 * (H * (p.z() ) / (H - h)) << " p.z() - h = " << p.z() - h << endl;
    const scalar pi = Foam::constant::mathematical::pi;
    scalar h = 0;
    scalar theta = Foam::atan2(p.y(),p.x());
    if (theta > pi/2 and theta < 9*pi/16) {
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
