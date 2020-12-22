#include "daisysp.h"
#include "super_petal.h"

using namespace daisysp;
using namespace daisy;

static SuperPetal sp;

float hardClip(float in)
{
    in = in > 1.f ? 1.f : in;
    in = in < -1.f ? -1.f : in;
    return in;
}

float softClip(float in)
{
    if(in > 0)
        return 1 - expf(-in);
    return -1 + expf(in);
}

bool        bypassHard, bypassSoft;
static void AudioCallback(float **in, float **out, size_t size)
{
    sp.ProcessAnalogControls();
    sp.ProcessDigitalControls();

    float Pregain = sp.knob[0].Process() * 10 + 1.2;
    float Gain    = sp.knob[1].Process() * 100 + 1.2;
    float drywet  = sp.knob[8].Process();

    bypassSoft = sp.switches[0].EitherEdge() ? !bypassSoft : bypassSoft;
    bypassHard = sp.switches[1].EitherEdge() ? !bypassHard : bypassHard;

    for(size_t i = 0; i < size; i++)
    {
        for(int chn = 0; chn < 2; chn++)
        {
            in[chn][i] *= Pregain;
            float wet = in[chn][i];

            if(!bypassSoft || !bypassHard)
            {
                wet *= Gain;
            }

            if(!bypassSoft)
            {
                wet = softClip(wet);
            }

            if(!bypassHard)
            {
                wet = hardClip(wet);
            }

            out[chn][i] = wet * drywet + in[chn][i] * (1 - drywet);
        }
    }
}

int main(void)
{
    sp.Init();

    bypassHard = bypassSoft = false;

    // start callback
    sp.StartAdc();
    sp.StartAudio(AudioCallback);
    while(1)
    {
        //LED stuff
        sp.SetFootswitchLed(0, !bypassSoft);
        sp.SetFootswitchLed(1, !bypassHard);
        sp.UpdateLeds();
        dsy_system_delay(6);
    }
}