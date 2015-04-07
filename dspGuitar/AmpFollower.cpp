#include "AmpFollower.h"

AmpFollower::AmpFollower(int fs){
	sampRate = fs;

	//set default values for filter coeffs
        //well need to calculate proper filter coeffs in matlab and set them here
        //since 48000 is larger than MAX16
	rmsa1 = 0;
	rmsb0 = MAX16;
	pa1A = 0;
	pb0A = MAX16;
	pa1R = 0;
	pb0R = MAX16;
        
	//initialise level estimates
	rmsLevel = 0;
	peakLevel = 0;
}

AmpFollower::~AmpFollower(){
}

int AmpFollower::rmsEnvelope(int inL, int inR){
	int amp = max(abs16(inL), abs16(inR));
	rmsLevel += mult16(rmsb0, (mult16(amp, amp) - rmsLevel));
	return sqrt(rmsLevel);
}

int AmpFollower::peakEnvelope(int inL, int inR){
	int amp = max(abs16(inL), abs16(inR));
	if(amp > peakLevel)
		peakLevel += mult16(pb0A, (amp - peakLevel));
	else
		peakLevel += mult16(pb0R, (amp - peakLevel));

	return peakLevel;
}
