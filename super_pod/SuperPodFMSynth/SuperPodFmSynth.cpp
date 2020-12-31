#include "daisysp.h"
#include "super_pod.h"
#include "FMSynth.h"
#include "myUtils/mathHelpers.h"

using namespace daisysp;
using namespace daisy;

// DaisySuperPod FM Synthesizer

SuperPod sp;
size_t num_channels;

FMSynth fm_synth;
Oscillator lfo_osc;
float sample_rate;
int encoder1_operator;

#define NUM_FREQ_MULT_VALUES 21
const float freq_mult_values[] = {0.0625f, 0.0833333f, 0.125f, 0.166667f, 0.25f, 0.333333f, 0.5f, 0.666667f, 0.75f, 0.8f, 1.0f, 1.25f, 1.33333f, 1.5f, 2.0f, 3.0f, 4.0f, 6.0f, 8.0f, 12.0f, 16.0f};

Parameter param[12];

static void AudioCallback(float **in, float **out, size_t size)
{
    float carrier_freq;
    float lfo_depth;
    float lfo_bias;
    float lfo_gain;
    float lfo_val;
    float sig;
   
    sp.ProcessAnalogControls();
    sp.ProcessDigitalControls();
  
    // Encoder 0 used to set the operator topology
    if (sp.encoder[0].ValueChanged())
    {
        fm_synth.SetOperatorTopology(sp.encoder[0].Value());
    }

    // Encoder 1 used to set waveform of the operators
    if (sp.encoder[1].ValueChanged())
    {
        fm_synth.op[encoder1_operator].SetWaveform(sp.encoder[1].Value());
    }

    // Buttons used to select which operator is controlled by encoder 1
    if (sp.buttons[0].RisingEdge())
    {
        encoder1_operator = 0;
        sp.encoder[1].SetValue(fm_synth.op[0].Waveform());
    }

    if (sp.buttons[1].RisingEdge())
    {
        encoder1_operator = 1;
        sp.encoder[1].SetValue(fm_synth.op[1].Waveform());
    }

    if (sp.buttons[2].RisingEdge())
    {
        encoder1_operator = 2;
        sp.encoder[1].SetValue(fm_synth.op[2].Waveform());
    }

    if (sp.buttons[3].RisingEdge())
    {
        encoder1_operator = 3;
        sp.encoder[1].SetValue(fm_synth.op[3].Waveform());
    }

    // Switches used to changed from continous to discrete frequency multipliers
    if (sp.switches[1].RisingEdge())
    {
        param[1].SetCurve(Parameter::MAP);
    }
    if (sp.switches[1].FallingEdge())
    {
        param[1].SetCurve(Parameter::LOGARITHMIC);
    }

    if (sp.switches[2].RisingEdge())
    {
        param[2].SetCurve(Parameter::MAP);
    }
    if (sp.switches[2].FallingEdge())
    {
        param[2].SetCurve(Parameter::LOGARITHMIC);
    }

    if (sp.switches[3].RisingEdge())
    {
        param[3].SetCurve(Parameter::MAP);
    }
    if (sp.switches[3].FallingEdge())
    {
        param[3].SetCurve(Parameter::LOGARITHMIC);
    }

    // FM Operator 0
    carrier_freq = mtof(param[0].Process());
    fm_synth.op[0].SetFreq(carrier_freq);
    fm_synth.op[0].SetAmp(param[4].Process());

    // FM Operator 1
    fm_synth.op[1].SetFreq(carrier_freq * param[1].Process());
    fm_synth.op[1].SetAmp(param[5].Process());

    // FM Operator 2
    fm_synth.op[2].SetFreq(carrier_freq * param[2].Process());
    fm_synth.op[2].SetAmp(param[6].Process());

    // FM Operator 3
    fm_synth.op[3].SetFreq(carrier_freq * param[3].Process());
    fm_synth.op[3].SetAmp(param[7].Process());

    // LFO
    lfo_osc.SetFreq(param[8].Process());
    lfo_depth = param[9].Process();
    lfo_bias = param[10].Process();
    lfo_gain = param[11].Process();
  
    // Audio Loop
    for(size_t i = 0; i < size; i ++)
    {
        // First get LFO as value in range 0-1
        lfo_val = gain(bias(lfo_osc.Process() + 0.5f, lfo_bias), lfo_gain);
        
        // Modify LFO using lfo_depth to get final value
        sig = (lfo_depth * (lfo_val - 1.0) + 1.0f) * fm_synth.Process();
        
        for (size_t chn = 0; chn < num_channels; chn++)
        {
            out[chn][i] = sig;
        }
    }
}

int main(void)
{
    sp.Init();
    num_channels = sp.NumChannels();
    sample_rate = sp.AudioSampleRate();

    // Encoder 0 is used to set the operator topology
    sp.encoder[0].SetRange(0, NUM_OPERATOR_TOPOLOGIES - 1);

    // Encoder 1 is used to set operator waveform. The buttons
    // are used to select which operator is currently being set
    sp.encoder[1].SetRange(0, NUM_OPERATOR_WAVEFORMS - 1);

    // Which operator encoder 1 currently controls
    encoder1_operator = 0;

    // Carrier pitch (in MIDI note values)
    param[0].Init(sp.knob[0], 0, 120, Parameter::LINEAR);

    // Relative frequencies of operators.
    // We set up the parameters so that they can be switched from MAP to LOGARITHMIC mode
    // We use the switch values to decide which one to use initially
    param[1].Init(sp.knob[1], 0.125f, 8.0f, sp.switches[1].Pressed() ? Parameter::MAP : Parameter::LOGARITHMIC);
    param[1].SetMapValues(NUM_FREQ_MULT_VALUES, freq_mult_values);

    param[2].Init(sp.knob[2], 0.125f, 8.0f, sp.switches[2].Pressed() ? Parameter::MAP : Parameter::LOGARITHMIC);
    param[2].SetMapValues(NUM_FREQ_MULT_VALUES, freq_mult_values);

    param[3].Init(sp.knob[3], 0.125f, 8.0f, sp.switches[3].Pressed() ? Parameter::MAP : Parameter::LOGARITHMIC);
    param[3].SetMapValues(NUM_FREQ_MULT_VALUES, freq_mult_values);

    param[4].Init(sp.knob[4], 0.0f, 1.0f, Parameter::LINEAR);

    // Amplitude of operators
    param[5].Init(sp.knob[5], 0.0f, 10.0f, Parameter::QUADRATIC);
    param[6].Init(sp.knob[6], 0.0f, 10.0f, Parameter::QUADRATIC);
    param[7].Init(sp.knob[7], 0.0f, 10.0f, Parameter::QUADRATIC);

    // LFO frequency and amplitude
    param[8].Init(sp.knob[8], 0.2f, 20.0f, Parameter::LOGARITHMIC);   // LFO frequency
    param[9].Init(sp.knob[9], 0.0f, 1.0f, Parameter::LINEAR);       // LFO depth
    param[10].Init(sp.knob[10], 0.001f, 0.995f, Parameter::LINEAR);  // LFO bias
    param[11].Init(sp.knob[11], 0.9f, 0.1f, Parameter::LINEAR);      // LFO gai

    // Initialise the FM oscillator
    fm_synth.Init(sample_rate);
    fm_synth.SetOperatorTopology(0);

    // Set feedback only on operator 3
    fm_synth.SetOperatorFeedback(3, true);

    // Initialise the LFO
    lfo_osc.Init(sample_rate);
    lfo_osc.SetWaveform(Oscillator::WAVE_SIN);
    lfo_osc.SetAmp(0.5f);

    // start callbacks
    sp.StartAdc();
    sp.StartAudio(AudioCallback);

    while(1) {
        sp.SetLedValue(0, sp.encoder[0].Value());
        sp.SetLedValue(1, sp.encoder[1].Value());
        sp.UpdateLeds();
        dsy_system_delay(6);
    }
}
