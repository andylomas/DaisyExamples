#include "daisysp.h"
#include "super_petal.h"

using namespace daisysp;
using namespace daisy;

#define NUM_WAVEFORMS 5

#define MAX_DELAY static_cast<size_t>(48000 * 0.5f)
static DelayLine<float, MAX_DELAY> flanger_dl;

SuperPetal sp;
 
Parameter flanger_rate_param;
Parameter flanger_delay_param;
Parameter flanger_depth_param;
Parameter flanger_feedback_param;
Parameter flanger_mix_param;

Oscillator flanger_lfo;

bool bypass;

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
    float del_time;
    float del_out;

    float flanger_rate;
    float flanger_mix;
    float flanger_delay;
    float flanger_depth;
    float flanger_feedback;

    sp.ProcessAnalogControls();
    sp.ProcessDigitalControls();

    int waveform = sp.encoder[0].Value();
    flanger_lfo.SetWaveform(waveforms[waveform]);

    flanger_rate = flanger_rate_param.Process();
    flanger_delay = flanger_delay_param.Process();
    flanger_depth = flanger_depth_param.Process();
    flanger_feedback = flanger_feedback_param.Process();
    flanger_mix = bypass ? 0.0f : flanger_mix_param.Process();

    flanger_lfo.SetFreq(flanger_rate);

    bypass = sp.switches[0].RisingEdge() ? !bypass : bypass;

    // Audio Loop
    for(size_t i = 0; i < size; i ++)
    {
        // Get dry signal
        dry = in[0][i];
                                                                                                                                                                                                                       
        // Process flanging to create the signal
        del_time = flanger_delay * (1.0f + flanger_depth * flanger_lfo.Process());
        //del_time = 100.0f * (1.0f + flanger_depth * flanger_lfo.Process());
        flanger_dl.SetDelay(del_time);
        del_out = flanger_dl.Read();
        wet = dry + flanger_mix * del_out;
        flanger_dl.Write(dry + flanger_feedback * del_out);

        // Process outputs
        out[0][i] = wet;
        out[1][i] = wet;
    }
}

int main(void)
{
    sp.Init();
    float sample_rate = sp.AudioSampleRate();

    sp.encoder[0].SetRange(0, NUM_WAVEFORMS-1);
    sp.encoder[0].SetResetOnPressed();

    flanger_rate_param.Init(sp.knob[0], 0.2f, 10.0f, Parameter::LOGARITHMIC);
    flanger_delay_param.Init(sp.knob[1], 0, 0.002f * sample_rate, Parameter::LINEAR);
    flanger_depth_param.Init(sp.knob[2], 0, 1, Parameter::LINEAR);
    flanger_feedback_param.Init(sp.knob[3], 0, 1, Parameter::LINEAR);
    flanger_mix_param.Init(sp.knob[8], 0, 1, Parameter::LINEAR);

    flanger_lfo.Init(sample_rate);
    flanger_lfo.SetAmp(1.0f);
    flanger_lfo.SetFreq(10.0f);

    sp.StartAdc();
    sp.StartAudio(AudioCallback);
    
    while(1)
    {
        //LED stuff
        sp.SetFootswitchLed(0, !bypass);
        sp.SetEncoderLedValue(0, sp.encoder[0].Value());
        sp.UpdateLeds();
        dsy_system_delay(6);
    }
}
