scalar streamFunctionAt(const point& p, const scalar& u0, const scalar& H, const scalar& h)
{
    // Info << "hello from streamFunctionAT" << endl;
    // Info << "streamFunctionAT p = " << p << endl;
    // Info << "streamFunctionAT u0 = " << u0 << endl;
    // Info << "streamFunctionAT H = " << H << endl;
    // Info << "streamFunctionAT h = " << h << endl;
    if (p.z() > H)
        {
            return -u0 * p.z();
        }
    else
        {
            return -u0 * (H * (p.z() - h) / (H - h));
        }
};
