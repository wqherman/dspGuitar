#ifndef PITCHTRACK_H
#define PITCHTRACK_H

#include  <Dsplib.h>
#include  <math.h>
#include "FixedLib.h"

using namespace FixedLib;

class Pitchtrack{

public:
    //constructor and destructor
    Pitchtrack(int fs);
    ~Pitchtrack();
    
    /*function to find the fundamental pitch. This will look for the fft bin
      with the highest amplitude and return that bin number. we do this because
      we'll use the bin number to look up the corresponding epsilon value
    */
    int findPitch(int *inBuff, int bufferLength);
    
    /*function that takes the fft of our signal
    */
    void getFFT();
    
private:
    int sampRate;
    int pitch;
    int prevPitch;
    
    int downFactor;    //the downsampling factor
    int downSampLength;
    int filterLength;  //length of downsampling filter
    int *filterCoeffs; //coefficients of downsampling filter
    int *filterState;  //stored state of the downsampling filter
    int *downSamped;
    
    int *window;
    int *INFFT;
    int fftLength;
    int binFreq;
};
#endif
