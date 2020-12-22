// Basic Template for SuperPetal
#include "super_petal.h"

using namespace daisy;

SuperPetal sp;

void AudioCallback(float **in, float **out, size_t size)
{
    sp.ProcessDigitalControls();
    sp.ProcessAnalogControls();

    for(size_t i = 0; i < size; i++)
    {
        for(int chn = 0; chn < 2; chn++)
        {
            out[chn][i] = in[chn][i];
        }
    }
}

int main(void)
{
    sp.Init();
    sp.StartAdc();
    sp.StartAudio(AudioCallback);
    for(;;) {}
}
