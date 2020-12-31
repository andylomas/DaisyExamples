#include "daisysp.h"
#include "super_pod.h"

using namespace daisysp;
using namespace daisy;

#define NUM_WAVEFORMS 8

SuperPod   sp;
Oscillator osc;
Parameter  p_freq;

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

static float freq;
float        sig;
static int   waveform, octave;

static void AudioCallback(float *in, float *out, size_t size)
{
    sp.ProcessDigitalControls();

    waveform += sp.encoder[0].Increment();
    osc.SetWaveform(waveforms[waveform]);

    // convert MIDI to frequency and multiply by octave size
    octave += sp.encoder[1].Increment();
    freq = mtof(p_freq.Process() + (octave * 12));
    osc.SetFreq(freq);

    // Audio Loop
    for(size_t i = 0; i < size; i += 2)
    {
        // Process
        sig        = osc.Process();
        out[i]     = sig;
        out[i + 1] = sig;
    }
}

void InitSynth(float samplerate)
{
    // Init freq Parameter to knob1 using MIDI note numbers
    // min 10, max 127, curve linear
    p_freq.Init(sp.knob[0], 0, 127, Parameter::LINEAR);

    osc.Init(samplerate);
    osc.SetAmp(1.f);

    waveform = 0;
    octave   = 0;
}

int main(void)
{
    float samplerate;

    // Init everything
    sp.Init();
    samplerate = sp.AudioSampleRate();
    InitSynth(samplerate);

    // start callbacks
    sp.StartAdc();
    sp.StartAudio(AudioCallback);

    sp.encoder[0].SetRange(0, NUM_WAVEFORMS-1);
    sp.encoder[0].ResetOnPressed();

    sp.encoder[1].SetRange(0, 4);
    sp.encoder[1].ResetOnPressed();
    sp.encoder[1].SetValue(2);

    while(1) {
        sp.SetEncoderLedValue(0, sp.encoder[0].Value());
        sp.SetEncoderLedValue(1, sp.encoder[1].Value());
        sp.UpdateLeds();
        dsy_system_delay(6);
    }
}
