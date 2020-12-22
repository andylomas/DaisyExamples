#include "super_petal.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

SuperPetal sp;
Comb       comb;
Oscillator lfo;
CrossFade  fader;

bool  bypassOn;
float buf[9600];

Parameter lfoFreqParam, lfoAmpParam;
Parameter combFreqParam, combRevParam;
Parameter faderPosParam;

float targetCombFreq, combFreq;

void UpdateControls();

void AudioCallback(float **in, float **out, size_t size)
{
    UpdateControls();

    for(size_t i = 0; i < size; i++)
    {
        fonepole(combFreq, targetCombFreq, .0001f);
        float f = combFreq + lfo.Process() + 50.f;
        comb.SetFreq(f);

        float inf     = in[0][i];
        float process = comb.Process(in[0][i]);
        out[0][i] = out[1][i] = fader.Process(inf, process);
    }
}

int main(void)
{
    float samplerate;
    sp.Init();
    samplerate = sp.AudioSampleRate();

    lfoFreqParam.Init(sp.knob[0], 0, 2, Parameter::LINEAR);
    lfoAmpParam.Init(sp.knob[1], 0, 50, Parameter::LINEAR);
    combFreqParam.Init(sp.knob[2], 25, 300, Parameter::LOGARITHMIC);
    combRevParam.Init(sp.knob[3], 0, 1, Parameter::LINEAR);
    faderPosParam.Init(sp.knob[8], 0, 1, Parameter::LINEAR);

    lfo.Init(samplerate);
    lfo.SetAmp(1);
    lfo.SetWaveform(Oscillator::WAVE_SIN);

    for(int i = 0; i < 9600; i++)
    {
        buf[i] = 0.0f;
    }

    // initialize Comb object
    comb.Init(samplerate, buf, 9600);

    fader.Init();

    sp.StartAdc();
    sp.StartAudio(AudioCallback);

    while(1)
    {
        sp.ClearLeds();
        sp.SetFootswitchLed(0, !bypassOn);
        sp.UpdateLeds();
        dsy_system_delay(60);
    }
}


void UpdateControls()
{
    sp.ProcessDigitalControls();

    //knobs
    lfo.SetFreq(lfoFreqParam.Process());
    lfo.SetAmp(lfoAmpParam.Process());

    targetCombFreq = combFreqParam.Process();

    comb.SetRevTime(combRevParam.Process());

    fader.SetPos(faderPosParam.Process());
    if(bypassOn)
    {
        fader.SetPos(0);
    }

    //bypass switch
    if(sp.switches[0].EitherEdge())
    {
        bypassOn = !bypassOn;
    }
}
