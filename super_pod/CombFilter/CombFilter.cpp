#include "daisysp.h"
#include "super_pod.h"

using namespace daisysp;
using namespace daisy;

#define NUM_WAVEFORMS 5

#define MAX_DELAY static_cast<size_t>(48000 * 0.5f)
static DelayLine<float, MAX_DELAY> comb_dl;

SuperPod sp;
 
Parameter comb_delay_param;
Parameter comb_mult_param;
Parameter comb_mix_param;
Parameter osc_amp_param;

Oscillator osc;
bool feedbackMode = true;

uint8_t waveforms[NUM_WAVEFORMS] = {
    Oscillator::WAVE_SIN,
    Oscillator::WAVE_TRI,
    Oscillator::WAVE_SQUARE,
    Oscillator::WAVE_SAW,
    Oscillator::WAVE_RAMP,
};

void AudioCallback(float **in, float **out, size_t size)
{   
    float dry;
    float wet;
    float del_out;

    sp.ProcessAnalogControls();
    sp.ProcessDigitalControls();

    int osc_waveform = sp.encoder[0].Value();
    osc.SetWaveform(waveforms[osc_waveform]);

    float comb_delay = comb_delay_param.Process();
    float comb_mult = comb_mult_param.Process();
    float comb_mix = comb_mix_param.Process();

    // convert MIDI to frequency and multiply by octave size
    int octave = sp.encoder[1].Value();
    float freq = mtof(60.0 + (octave * 12));
    osc.SetFreq(freq);
    osc.SetAmp(osc_amp_param.Process());

    if (sp.button[0].RisingEdge()) feedbackMode = !feedbackMode;

    // Audio Loop
    for(size_t i = 0; i < size; i ++)
    {
        // Get dry signal
        dry = osc.Process();
                                                                                                                                                                                                                       
        // Process flanging to create the signal
        comb_dl.SetDelay(comb_delay);
        del_out = comb_dl.Read();
        wet = dry + comb_mult * del_out;

        if (feedbackMode)
            comb_dl.Write(wet);
        else
            comb_dl.Write(dry);

        float sig = comb_mix * wet + (1.0f - comb_mix) * dry;

        // Process outputs
        out[1][i] = sig;
        out[0][i] = sig;
    }
}

int main(void)
{
    sp.Init();
    float sample_rate = sp.AudioSampleRate();

    sp.encoder[0].SetRange(0, NUM_WAVEFORMS-1);
    sp.encoder[0].ResetOnPressed();

    sp.encoder[1].SetRange(-3, 4);
    sp.encoder[1].ResetOnPressed();

    osc.Init(sample_rate);

    comb_delay_param.Init(sp.knob[0], 0, 0.1f * sample_rate, Parameter::LINEAR);
    comb_mult_param.Init(sp.knob[1], 0, 1, Parameter::LINEAR);
    comb_mix_param.Init(sp.knob[2], 0, 1, Parameter::LINEAR);
    osc_amp_param.Init(sp.knob[3], 0, 1, Parameter::LINEAR);

    sp.StartAdc();
    sp.StartAudio(AudioCallback);
    
    while(1)
    {
        //LED stuff
        sp.SetEncoderLedValue(0, sp.encoder[0].Value());
        sp.SetEncoderLedValue(1, sp.encoder[1].Value());
        sp.UpdateLeds();
        dsy_system_delay(100);
    }
}
