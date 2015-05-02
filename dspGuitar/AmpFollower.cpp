#include "AmpFollower.h"

AmpFollower::AmpFollower(int fs)
{
	sampRate = fs;

	//set default values for filter coeffs
        //well need to calculate proper filter coeffs in matlab and set them here
        //since 48000 is larger than MAX16
	rmsa1 = 32765;  //corresponds in floating point to: round(exp(-1 / (0.2*48000))*2^15)
	rmsb0 = MAX16 - rmsa1;
	pa1A = 32700;  //corresponds in floating point to: round(exp(-1 / (0.05*48000))*2^15)
	pb0A = MAX16 - pa1A;
	pa1R = 32765;  //corresponds in floating point to: round(exp(-1 / (0.2*48000))*2^15)
	pb0R = MAX16 - pa1R;
        
	//initialise level estimates
	rmsLevel = 0;
	peakLevel = 0;
        prevPeakLevel = 0;
}

AmpFollower::~AmpFollower(){}

int AmpFollower::rmsEnvelope(int inL, int inR)
{
	int amp = max(abs16(inL), abs16(inR));
	rmsLevel += mult16(rmsb0, (mult16(amp, amp) - rmsLevel));
	return sqrt(rmsLevel); //todo: double check this sqrt function and make sure it properly operates on fixed point
}

int AmpFollower::peakEnvelope(int inL, int inR)
{
    int amp = abs16(inL);
    if(amp < 400)
    {
        return 0;
    }
    peakLevel += mult16(pb0A, (amp - peakLevel));
    return peakLevel;
}
