#include "Pitchtrack.h"

Pitchtrack::Pitchtrack(int fs)
{
    //set internal sampling rate
    sampRate = fs;
    
    //set filter length, downsampling factor and initialize filter state
    //these are all parameters determined in matlab
    fftLength = 2048;
    downFactor = 8;
    downSampLength = 32;
    filterLength = 121;
    binFreq = mult16(6000, 16); //the downsampled fft rate times 1/fftLength calculated in matlab so as to not use a division
    filterCoeffs = new int[filterLength];
    filterState = new int[filterLength+2];
    downSamped = new int[downSampLength];  //this is the buffer length / downFactor, hard coded to save the division
    window = new int[downSampLength];      //windowing for the fft
    INFFT = new int[fftLength];
    prevPitch = 0;
    
    //these are the filter and window coefficients as determined by matlab, we'll copy these
    //into our member array (probably a cleaner way to do this)
    const int matlabCoeffs[121] = {-3,-16,-9,-13,-13,-13,-10,-5,2,10,20,29,38,44,47,44,37,23,5,-18,-44,-69,-92,-109,-118,-116,-101,-73,-33,17,74,133,187,230,257,262,240,191,114,13,-106,-234,-359,-469,-550,-590,-577,-501,-358,-147,130,463,839,1242,1653,2049,2409,2714,2945,3089,3138,3089,2945,2714,2409,2049,1653,1242,839,463,130,-147,-358,-501,-577,-590,-550,-469,-359,-234,-106,13,114,191,240,262,257,230,187,133,74,17,-33,-73,-101,-116,-118,-109,-92,-69,-44,-18,5,23,37,44,47,44,38,29,20,10,2,-5,-10,-13,-13,-13,-9,-16,-3};
    const int matlabWindow[32] = {0,335,1328,2937,5096,7717,10694,13903,17214,20491,23599,26413,28815,30709,32017,32684,32684,32017,30709,28815,26413,23599,20491,17214,13903,10694,7717,5096,2937,1328,335,0};
    
    //Set the filter coefficients and zero out the filter state
    for(int i = 0; i < downSampLength; i++)
    {
        window[i] = matlabWindow[i];
    }
    for(int i = 0; i < filterLength; i++)
    {
        filterCoeffs[i] = matlabCoeffs[i];
    }
    for(int i = 0; i < filterLength+2; i++)
    {
        filterState[i] = 0;
    }
}

Pitchtrack::~Pitchtrack(){};

void Pitchtrack::getFFT()
{
    //copy our downsampled input into the fft buffer and apply the window
    for(int i = 0; i < downSampLength; i++)
    {
        INFFT[i] = mult16(downSamped[i], window[i]);
    }
    //make sure the rest of the input fft array is zero
    for(int i = downSampLength; i < fftLength; i++)
    {
        INFFT[i] = 0;
    }
    
    //take the real fft
    //TODO: figure our the ordering of the fft, wording in documentation is a little confusing
    rfft((DATA *)INFFT, fftLength, SCALE);
}

int Pitchtrack::findPitch(int *input, int bufferLength)
{
    //downsample the input so we can get better resolution with less fft points
    firdec((DATA *)input, (DATA *)filterCoeffs, (DATA *)downSamped, (DATA *)filterState, bufferLength, filterLength, downFactor);

    //calculate the fft
    getFFT();

    //calculate the pitch based on the fft bins 
    int maxBin = 0;
    for(int i = 0; i < fftLength; i+=2)  //increment by 2 because data stored in re, im format
    {
        if(INFFT[i] > INFFT[maxBin])
            maxBin = i;
    } 
    pitch = mult16(maxBin, binFreq);
    
    //since every other entry in the fft array is real, we must divide by 2 to get appropriate epsilon bin number
    maxBin = mult16(16384, maxBin);
    if(maxBin < 28)
      maxBin = 28;
    if(maxBin > 550)
      maxBin = 550;
    //return the max bin, this is what we'll use to look up the corresponding
    //epsilon for the magic circle algorithm
    return maxBin;
}
