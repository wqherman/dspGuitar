/*-------------------DSPGUITAR--------------------------------
  This program uses the dsp sheild as an embedded processor for an electric guitar.
  The dsp shield can be switched to implement several effects:
      ->synth guitar? (tracks envelope and pitch and plays synthesized tones)
      
      
*/

#include  "Audio_new.h"
#include  "AmpFollower.h"
#include  "Pitchtrack.h"
#include  "SinOsc.h"
#include  <OLED.h>
#include  <filter.h>

using namespace FixedLib;
//-----------Global Variables---------------------------
const int BufferLength = 256;
const int NoiseLength = 2048;

//input and output buffers
int *InputLeft;  
int *InputRight;
int *OutputLeft;
int *OutputRight;

//variables to read through our noise buffer for testing amplitude tracker
int noiseIndex = 0;
int *noiseReader = noise;

//fft bin of the found pitch
int carrierPitchBin;  //bin number of our carrier (corresponds to location of epsilon)
int modPitchBin;      //bin number of modulator
int modDepth;        //depth of the frequency modulation
int buffCounter;    //a buffer counter to make sure our pitch doesn't change too fast
int wetDry;        //controls the wet dry mix of the guitar and the fm modulated signal

//level tracker to follow envelope of guitar
AmpFollower guitarEnvelope(48000);

//instance of our pitch tracker class
Pitchtrack guitarPitch(48000);

//our sinoscillators
/*NOTE:
  BY CREATING AN ARRAY OF THESE AND INCREMENTING THROUGH THE ARRAY EVERYTIME
  A NEW PITCH IS CALCULATED, WE COULD POENTIALLY MAKE THIS SEMI-POLYPHONIC
  */
SinOsc carrier(48000, 80);
SinOsc modulator(48000, 80);

//setup pins and values for pots
int faderPin = 2;
int knobLPin = 1;
int knobRPin = 3;

//------------------------------------------------------
void setup()
{
  int status;
  int index;
  digitalWrite(LED0, HIGH);
  
  //Initialize OLED 
  disp.oledInit();
  disp.clear();
  disp.flip();
  disp.setline(1);
  carrier.changeFreq(1887);
  //allocate space for our input and output buffers
  InputLeft = new int[BufferLength];
  InputRight = new int[BufferLength];
  OutputLeft = new int[BufferLength];
  OutputRight = new int[BufferLength];
  
  //set up the audio library in non loop back mode
  status = AudioC.Audio(TRUE, BufferLength, BufferLength);
  AudioC.setSamplingRate(SAMPLING_RATE_48_KHZ);
  AudioC.setInputGain(40, 40);
  AudioC.setOutputVolume(75, 75);
 
 //check to make sure the audio has been set up properly and if not print warning
  if(status == 0)
  {
      disp.print("Guitar!");
      AudioC.attachIntr(dmaIsr);
  }
  
  //set initial values for moddepth and wetdry mix
  wetDry = 32760;
  modDepth = 30;
  carrierPitchBin = 80;
  modPitchBin = 50;
  
  //set sine wave frequency  
  modulator.changeFreq(modPitchBin);
  carrier.changeFreq(carrierPitchBin);
}

void loop()
{  
  //read values from our pots
  carrierPitchBin = (int)(analogRead(faderPin));
  wetDry = 20000 + (int)(analogRead(knobLPin) * 10);
  modPitchBin = (int)analogRead(knobRPin);
}

/*--------Processing Function----------------------
  Where samples get processed, essentially the audio callback function.
*/
void processData(int* inputLeft, int* inputRight, int *outputLeft, int *outputRight)
{
    //variables for our envelopes
    int rmsEnv;
    int peakEnv;  

    //for now just pass input to output 
    for(int i = 0; i < BufferLength; i++)
    {   
        //dtermine the frequency as a function of our modulator and calculated pitch
        modulator.changeFreq(modPitchBin);      //be careful with chaning modulator frequencies can lead to ugliness
        int modValue = mult16(modDepth, modulator.nextSampleMagic());
        carrier.changeFreq(carrierPitchBin + modValue);
        
        //generate modulated sine wave
        int sinValue = carrier.nextSampleMagic();
        
        //modulate our input signal, then envelope it with the envelope of the guitar to try and get rid of background noise
        outputLeft[i] = mult16(wetDry, mult16(abs16(inputLeft[i]), mult16(inputLeft[i], sinValue))); 
        outputRight[i] = mult16(wetDry, mult16(abs16(inputLeft[i]), mult16(inputLeft[i], sinValue))); 
        outputLeft[i] += mult16(MAX16 - wetDry, inputLeft[i]);
        outputRight[i] += mult16(MAX16 - wetDry, inputLeft[i]);
    }  
}

/*--------Audio Interrupt---------------------------
  Handles getting a buffer length of data from the input buffers and sending the output
  to the output buffers. Calls the process data function to apply any processing needed.
*/
interrupt void dmaIsr(void)
{
    unsigned short ifrValue;

    // Variable to indicate that the Input samples are ready for processing
    static unsigned short readyToProcess = 0;
    
    // Variable to indicate that the processing is completed and that valid
    // data is available "output" buffer
    static unsigned short outputBufAvailable = 0;
    
    // Variable to switch between the data buffers of the Audio library
    static unsigned short writeBufIndex = 0;

    ifrValue = DMA.getInterruptStatus();
    if ((ifrValue >> DMA_CHAN_ReadR) & 0x01)
    {
        /* Data read from codec is copied to process input buffers.
           Processing is done after configuring DMA for next block of transfer
           ensuring no data loss */
        copyShortBuf(AudioC.audioInLeft[AudioC.activeInBuf],
                     InputLeft, BufferLength);
        copyShortBuf(AudioC.audioInRight[AudioC.activeInBuf],
                     InputRight, BufferLength);
        readyToProcess = 1;
    }
    else if ((ifrValue >> DMA_CHAN_WriteR) & 0x01)
    {
        if (outputBufAvailable)
        {
            /* Processed buffers need to be copied to audio out buffers as
               audio library is configured for non-loopback mode */
            writeBufIndex = (AudioC.activeOutBuf == FALSE)? TRUE: FALSE;
            copyShortBuf(OutputLeft, AudioC.audioOutLeft[writeBufIndex],
                         BufferLength);
            copyShortBuf(OutputRight, AudioC.audioOutRight[writeBufIndex],
                         BufferLength);
            outputBufAvailable = 0;
        }
    }

    /* Calling AudioC.isrDma() will copy the buffers to Audio Out of the Codec,
     * initiates next DMA transfers of Audio samples to and from the Codec
     */
    AudioC.isrDma();

    // Check if output buffers are ready. No processing required for write interrupt
    if (readyToProcess)
    {
        readyToProcess = 0;

        processData(InputLeft, InputRight, OutputLeft, OutputRight);

        outputBufAvailable = 1;
    }
}
