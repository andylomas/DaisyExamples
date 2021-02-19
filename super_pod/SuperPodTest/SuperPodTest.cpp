#include "daisysp.h"
#include "super_pod.h"

using namespace daisysp;
using namespace daisy;

#define NUM_WAVEFORMS 8

SuperPod   sp;
Oscillator osc;

int count = 0;

uint8_t waveforms[NUM_WAVEFORMS] = {
    Oscillator::WAVE_SIN,
    Oscillator::WAVE_TRI,
    Oscillator::WAVE_SAW,
    Oscillator::WAVE_RAMP,
    Oscillator::WAVE_SQUARE,
    Oscillator::WAVE_POLYBLEP_TRI,
    Oscillator::WAVE_POLYBLEP_SAW,
    Oscillator::WAVE_POLYBLEP_SQUARE,
};

static void AudioCallback(float **in, float **out, size_t size)
{
    sp.ProcessAnalogControls();
    sp.ProcessDigitalControls();

    int waveform = sp.encoder[0].Value();
    osc.SetWaveform(waveforms[waveform]);

    float basenote = 10.0;
    for (int i = 0; i < 12; i++)
    {
        basenote += 4.0 * sp.knob[i].Value();
    }

    osc.SetFreq(mtof(basenote
                    + 2 * (sp.button[0].Pressed() + 2 * sp.button[1].Pressed() + 4 * sp.button[2].Pressed() + 8 * sp.button[3].Pressed())
                    + 2 * (sp.switches[0].Pressed() + 2 * sp.switches[1].Pressed() + 4 * sp.switches[2].Pressed() + 8 * sp.switches[3].Pressed())));

    // Audio Loop
    for(size_t i = 0; i < size; i++)
    {
        // Process
        float sig = osc.Process();
        out[0][i] = sig;
        out[1][i] = sig;
    }
}

int main(void)
{
    float samplerate;

    // Init everything
    sp.Init();
    samplerate = sp.AudioSampleRate();

    sp.encoder[0].SetRange(0, NUM_WAVEFORMS-1);
    sp.encoder[1].SetRange(0, 4);

    osc.Init(samplerate);
    osc.SetAmp(1.f);
    osc.SetFreq(500.0f);

    // start callbacks
    sp.StartAdc();
    sp.StartAudio(AudioCallback);

    while(1) {
        sp.SetEncoderLedValue(0, sp.encoder[0].Value());
        sp.SetEncoderLedValue(1, sp.encoder[1].Value());
        sp.UpdateLeds();
        dsy_system_delay(6);
    }
}
