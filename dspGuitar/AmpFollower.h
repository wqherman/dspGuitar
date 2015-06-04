#ifndef AMPFOLLOWER_H
#define AMPFOLLOWER_H

#include  "FixedLib.h"
#include  <dsplib.h>
#include  <math.h>
using namespace FixedLib;

//-------Define some helpful functions-------------
#define max(a,b)			(((a) > (b)) ? (a) : (b))

#define min(a,b)			(((a) < (b)) ? (a) : (b))

//---------------------------------------------------

struct LevelDetector
{
    int b0_r, a1_r, b0_a, a1_a, levelEstimate;
    
    LevelDetector() {
      this->b0_r = 32768;
      this->a1_r = 0;
      this->b0_a = 32768;
      this->a1_a = 0;
      this->levelEstimate = 0;
    }
    
    void setRelease(int b, int a)
    {
      b0_r = b;
      a1_r = a;
    }
    
    void setAttack(int b, int a)
    {
      b0_a = b;
      a1_a = a;
    }
    
    int process(int input)
    {
      if(input >= levelEstimate)
        levelEstimate += mult16(b0_a, (input - levelEstimate));
      if(input < levelEstimate)
        levelEstimate += mult16(b0_r, (input - levelEstimate)); 
        
        return levelEstimate;
    }
};

class AmpFollower{

public:
	//constructor and destructor
	AmpFollower(int fs);
	~AmpFollower();

	//calculates the evelope using rms detection
	int rmsEnvelope(int inL, int inR);

	//calculates the peak detector envelope at the sample
	int peakEnvelope(int inL, int inR);

        //determines the presence of transients using the peak squared method
        int transient(int *inL, int *inR, int buffSize);
	
private:
        //two level detectors used for peak squared transient detection
	LevelDetector peakDetector1;
        LevelDetector peakDetector2;
        
        //a third lvl detecor for the actual amplitude envelope of our signal
        //LevelDetector peakEnvelope;
        
        int lvlEst1;
        int lvlEst2;
        int transientAvg;
        int prevTransientAvg;
        int transientLock;
        int transientSlope;
        int prevTransientSlope;
        
        //bullshit unused params to avoid compiler errors, TODO: cleanup
        int sampRate;
        int rmsLevel;
        int rmsb0;
};
#endif
