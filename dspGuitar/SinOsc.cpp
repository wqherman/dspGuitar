#include "SinOsc.h"

SinOsc::SinOsc(long fs, int freq = 0)
{
    //initialize member variables
    outputFs = fs;
    lowerFs = 6000;
    upFactor = 8;
    myFreq = freq;
    
    //initialize vairables for magic circle
    magicPrevX = 20000;
    magicPrevY = 0;
    magicEpsilon = 1887;
    
    //determine value of pi
    pi = 102944;   //calculated by using round(pi*2^15) in matlab must be a long value
    //think of other ways to determine pi    
}

SinOsc::~SinOsc(){};

void SinOsc::changeFreq(int freqBin)
{
   //look up our epsilon value from the table of epsilon values
   magicEpsilon = fixedEpsilon[freqBin];
}

int SinOsc::nextSampleWaveguide()
{
}

int SinOsc::nextSampleMagic()
{   
    magicx = magicPrevX + mult16(magicPrevY, magicEpsilon);
    magicy = magicPrevY - mult16(magicx, magicEpsilon);
    
    magicPrevX = magicx;
    magicPrevY = magicy;
    
    return magicy;
}
