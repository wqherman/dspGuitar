#include "AmpFollower.h"

AmpFollower::AmpFollower(int fs)
{
	sampRate = fs;

	//set default values for filter coeffs
        //well need to calculate proper filter coeffs in matlab and set them here
        //since 48000 is larger than MAX16
	int pa1A = 32632;  //corresponds in floating point to: round(exp(-1 / (0.01*48000))*2^15)
	int pb0A = MAX16 - pa1A;
	int pa1R = 32765;  //corresponds in floating point to: round(exp(-1 / (0.25*48000))*2^15)
	int pb0R = MAX16 - pa1R;

        //now pass the matlab coeffs into our detectors
        peakDetector1.setAttack(pb0A, pa1A);
        peakDetector1.setRelease(pb0R, pa1R);
        peakDetector2.setAttack(pb0A, pa1A);
        peakDetector2.setRelease(pb0R, pa1R);
        
        lvlEst1 = 0;
        lvlEst2 = 0;
        transientAvg = 0;
        prevTransientAvg = 0;
        transientLock = 0;
        transientSlope = 0;
        prevTransientSlope = 0;
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
    if(amp < 40)
    {
        return 0;
    }
    
    return peakDetector1.process(amp);   
}

int AmpFollower::transient(int *inL, int *inR, int buffSize)
{
    //peak squared method uses output of one level detector as input to another
    for(int i = 0; i < buffSize; i++)
    {
      int amp = abs16(inL[i]);
    
      lvlEst1 = peakDetector1.process(amp);
      lvlEst2 = peakDetector2.process(lvlEst1);
      
      //the amplitude is then averaged over a buffer
      transientAvg += lvlEst2;
    }
    
    //divide transient average by buffer size, this is equal to multiplying by 1/256 which is 128
    transientAvg = mult16(transientAvg, 128);
    
    //examine slope of averages, if it is greater than the previous slope, a transient has started
    transientSlope = transientAvg - prevTransientAvg;
    
    //if the slope is greater than the previous slope a transient will have started
    if(transientSlope > prevTransientSlope)
    {
      if(transientLock == 0)
      {
        transientLock = 1;  //set transient lock
        return 1;
      }
    }
    
    //when the slope is less than the previous slope we've ended our transient and can start searching for new ones
    if(transientSlope < prevTransientSlope)
      transientLock = 0;
    
    //update storage of previous values
    prevTransientAvg = transientAvg;
    prevTransientSlope = transientSlope;
    return 0;
}
