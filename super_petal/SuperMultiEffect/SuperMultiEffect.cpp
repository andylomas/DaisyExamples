// Basic Template for SuperPetal
#include "super_petal.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

SuperPetal sp;

Flanger flanger;

void AudioCallback(float **in, float **out, size_t size)
{
    sp.ProcessDigitalControls();
    sp.ProcessAnalogControls();

    for(size_t i = 0; i < size; i++)
    {
        float wet = flanger.Process(in[chn][0]);

        out[chn][0] = out[chn][1] = wet;
    }
}

int main(void)
{
    sp.Init();
    float sample_rate = sp.AudioSampleRate();

    flanger.Init(sample_rate);
    flanger.SetLfoDepth(.5f);
    flanger.SetFeedback(.83f);

    sp.StartAdc();
    sp.StartAudio(AudioCallback);

    bool led_state = true;
    while(1)
    {
        // Set the onboard LED
        sp.seed.SetLed(led_state);

        // Toggle the LED state for the next time around.
        led_state = !led_state;

        // Wait 500ms
        dsy_system_delay(500);
    }
}
