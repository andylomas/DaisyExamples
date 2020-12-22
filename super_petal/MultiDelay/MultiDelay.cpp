#include "daisysp.h"
#include "super_petal.h"
#include <string>

#define MAX_DELAY static_cast<size_t>(48000 * 1.f)

using namespace daisy;
using namespace daisysp;

SuperPetal petal;

DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS delMems[3];

struct delay
{
    DelayLine<float, MAX_DELAY> *del;
    float                        currentDelay;
    float                        delayTarget;
    float                        feedback;

    float Process(float in)
    {
        //set delay times
        fonepole(currentDelay, delayTarget, .0002f);
        del->SetDelay(currentDelay);

        float read = del->Read();
        del->Write((feedback * read) + in);

        return read;
    }
};

delay     delays[3];
Parameter params[3];

float feedback;
int   drywet;
bool  passThruOn;

void ProcessControls();

static void AudioCallback(float **in, float **out, size_t size)
{
    ProcessControls();

    for(size_t i = 0; i < size; i++)
    {
        float mix     = 0;
        float fdrywet = passThruOn ? 0.f : (float)drywet;

        //update delayline with feedback
        for(int d = 0; d < 3; d++)
        {
            float sig = delays[d].Process(in[0][i]);
            mix += sig;
        }

        //apply drywet and attenuate
        mix       = fdrywet * mix * .3f + (1.0f - fdrywet) * in[0][i];
        out[0][i] = out[1][i] = mix;
    }
}

void InitDelays(float samplerate)
{
    for(int i = 0; i < 3; i++)
    {
        //Init delays
        delMems[i].Init();
        delays[i].del = &delMems[i];
        //3 delay times
        params[i].Init(petal.knob[i],
                       samplerate * .05,
                       MAX_DELAY,
                       Parameter::LOGARITHMIC);
    }
}

int main(void)
{
    float samplerate;
    petal.Init(); // Initialize hardware (daisy seed, and petal)
    samplerate = petal.AudioSampleRate();

    InitDelays(samplerate);

    drywet     = 50;
    passThruOn = false;

    petal.StartAdc();
    petal.StartAudio(AudioCallback);
    while(1)
    {
        // int32_t whole;
        // float   frac;
        // whole = (int32_t)((float)drywet / 12.5f);
        // frac  = (float)drywet / 12.5f - whole;
        // petal.ClearLeds();

        // // Set full bright
        // for(int i = 0; i < whole; i++)
        // {
        //     petal.SetLed(i, 0, 0, 1);
        // }

        // // Set Frac
        // if(whole < 7 && whole > 0)
        //     petal.SetLed(
        //         whole - 1, 0, 1, 0);

        // Update Pass thru
        petal.SetFootswitchLed(0, !passThruOn);
        petal.UpdateLeds();
        dsy_system_delay(6);
    }
}

void ProcessControls()
{
    petal.ProcessAnalogControls();
    petal.ProcessDigitalControls();

    //knobs
    for(int i = 0; i < 3; i++)
    {
        delays[i].delayTarget = params[i].Process();
        delays[i].feedback    = petal.knob[i + 4].Process();
    }

    drywet = petal.knob[8].Process();

    //footswitch
    if(petal.switches[0].EitherEdge())
    {
        passThruOn = !passThruOn;
    }
}
