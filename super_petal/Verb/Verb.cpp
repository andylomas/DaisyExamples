#include "super_petal.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

// Declare a local daisy_petal for hardware access
SuperPetal sp;

Parameter vtime, vfreq, vsend;
bool      bypass;
ReverbSc  verb;

// This runs at a fixed rate, to prepare audio samples
void callback(float *in, float *out, size_t size)
{
    float dryl, dryr, wetl, wetr, sendl, sendr;
    sp.ProcessDigitalControls();
    verb.SetFeedback(vtime.Process());
    verb.SetLpFreq(vfreq.Process());
    vsend.Process(); // Process Send to use later
    if(sp.switches[0].RisingEdge())
        bypass = !bypass;
    for(size_t i = 0; i < size; i += 2)
    {
        dryl  = in[i];
        dryr  = in[i + 1];
        sendl = dryl * vsend.Value();
        sendr = dryr * vsend.Value();
        verb.Process(sendl, sendr, &wetl, &wetr);
        if(bypass)
        {
            out[i]     = in[i];     // left
            out[i + 1] = in[i + 1]; // right
        }
        else
        {
            out[i]     = dryl + wetl;
            out[i + 1] = dryr + wetr;
        }
    }
}

int main(void)
{
    float samplerate;

    sp.Init();
    samplerate = sp.AudioSampleRate();

    vtime.Init(sp.knob[0], 0.6f, 0.999f, Parameter::LOGARITHMIC);
    vfreq.Init(sp.knob[1], 500.0f, 20000.0f, Parameter::LOGARITHMIC);
    vsend.Init(sp.knob[8], 0.0f, 1.0f, Parameter::LINEAR);
    verb.Init(samplerate);

    sp.StartAdc();
    sp.StartAudio(callback);
    while(1)
    {
        // Do Stuff InfInitely Here
        dsy_system_delay(10);
        sp.ClearLeds();
        sp.SetFootswitchLed(0, !bypass);
        sp.UpdateLeds();
    }
}
