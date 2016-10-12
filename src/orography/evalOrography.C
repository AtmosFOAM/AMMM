#include "fvCFD.H"
#include "setInternalValues.H"
#include "evalOrography.H"

scalar evalOrography(point& p){
    scalar x = p.x();
    //scalar y = p.y();

    bool exponential=false;
    bool witchOfAgnesi=true;

    // the orography to be output
    scalar h=0; 
    if(exponential){
        //const scalar tau = 2.0*Foam::constant::mathematical::pi;
                
        // this is for an exponential mountain
        scalar centre = -6.0;
        // set the full width at half maximum
        scalar FWHM = 2.0;
        // set the maximum height
        scalar maxH = 1.;

        //scalar domainLength = 10.0;
        scalar sigma2 = sqr(FWHM)/ (4.0*(2.0*Foam::log(2.0)));
        //scalar dis = std::min(std::abs(x-centre),domainLength-std::abs(x-centre));
        scalar dis = std::abs(x-centre);
    
        h = maxH*( std::exp( -sqr(dis)/(2*sigma2)) );
    }
    if (witchOfAgnesi) {
        scalar xm = -6.0;
        scalar a = 1.0;
        scalar H = 1.;
        h = H/(1+sqr((x-xm)/a));
    }
    return h;                          
}





