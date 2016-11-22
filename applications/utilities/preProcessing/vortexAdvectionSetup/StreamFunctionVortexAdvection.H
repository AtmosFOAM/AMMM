scalar streamFunctionVortexAdvection(const point& p, const scalar& u0, const scalar& Htop, const scalar& orog_height, const scalar& orog_start, const scalar& orog_stop, const scalar& dx)
{
    scalar psi_c = 0.0;
    scalar psi_v = 0.0;
    scalar h = 0;
    scalar theta = p.x()/dx;
    if (theta > orog_start and theta < orog_stop) {
        h = orog_height;
    }
    if (p.z() > Htop)
        {
            psi_c = -u0 * p.z();
        }
    else
        {
            psi_c =  -u0 * (Htop * (p.z() - h) / (Htop - h));
        }



    scalar width = 10000;
    point x_v = vector(10000,0,7500);
    scalar u_v = 60000;
    psi_v = -u_v * (-mag(p-x_v)/width);

    psi_v = -u_v * Foam::exp(-(mag(p-x_v)/width));

    Info << "psi_v at point " << p << " is " << psi_v << endl;



    return psi_v;
};
