#ifndef SINOSC_H
#define SINOSC_H

#include  <Dsplib.h>
#include "FixedLib.h"

using namespace FixedLib;

/*  A sine oscillator class that will be implemented using the 
    the digital waveguide oscillator. Because the output sampling
    rate is 48k, which is higher than MAX16, we will generate the
    sine waves at the same rate as used by the pitch tracking class
    and then upsample on output
*/
class SinOsc{
  
public:
    //constructor and destructor
    SinOsc(long fs, int freqBin);
    ~SinOsc();
    
    //changes the frequency of the oscillator by looking up an
    //epsilon value with the frequency bin obtained from the pitch
    //track class
    void changeFreq(int freqBin);
    
    //outputs next sample of oscillator using digital waveguide
    int nextSampleWaveguide();
    
    //outputs next sample of oscillator using magic circle algorithm
    int nextSampleMagic();
    
private:
    int lowerFs;
    long outputFs;
    int upFactor;
    int posCounter;
    int myFreq;
    
    //vairables for magic circle
    int magicEpsilon;
    int magicx;
    int magicy;
    int magicPrevX;
    int magicPrevY;
    
    //value for pi
    long pi;
};
#endif
