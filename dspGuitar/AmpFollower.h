#ifndef AMPFOLLOWER_H
#define AMPFOLLOWER_H

#include  "FixedLib.h"
#include  <Dsplib.h>
#include  <math.h>
using namespace FixedLib;

//-------Define some helpful functions-------------
#define max(a,b)			(((a) > (b)) ? (a) : (b))

#define min(a,b)			(((a) < (b)) ? (a) : (b))

//---------------------------------------------------

class AmpFollower{

public:
	//constructor and destructor
	AmpFollower(int fs);
	~AmpFollower();

	//calculates the evelope using rms detection
	int rmsEnvelope(int inL, int inR);

	//calculates the peak detector envelope at the sample
	int peakEnvelope(int inL, int inR);
	
private:
	int rmsTau;
	int peakTauA;
	int peakTauR;
	int rmsa1, rmsb0;	//coeffs of rms leaky integrator
	int pa1A, pb0A, pa1R, pb0R; //coeffs of peak leaky integrator
	int sampRate;
	int rmsLevel;
	int peakLevel;
};
#endif
