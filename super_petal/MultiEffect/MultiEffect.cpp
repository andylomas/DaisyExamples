#include "daisysp.h"
#include "super_petal.h"

// Set max delay time to 0.75 of samplerate.
#define MAX_DELAY static_cast<size_t>(48000 * 2.5f)

using namespace daisysp;
using namespace daisy;

static SuperPetal sp;

static ReverbSc                                  rev;
static DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS dell;
static DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS delr;
static Autowah                                   wah[2];
static Parameter deltime, reverbLpParam, crushrate;

float currentDelay, feedback, delayTarget, cutoff, dryWet[5];

int   crushmod, crushcount;
float crushl, crushr;
bool  effectOn[4];

//Helper functions
void Controls();
void UpdateLeds();

void GetCrushSample(float &inl, float &inr);
void GetWahSample(float &inl, float &inr);
void GetDelaySample(float &inl, float &inr);
void GetReverbSample(float &inl, float &inr);


enum effectTypes
{
    CRUSH,
    WAH,
    DEL,
    REV,
    ALL,
    LAST,
};
effectTypes dryWetMode;

void GetSample(float &inl, float &inr, effectTypes type)
{
    switch(type)
    {
        case CRUSH: GetCrushSample(inl, inr); break;
        case WAH: GetWahSample(inl, inr); break;
        case DEL: GetDelaySample(inl, inr); break;
        default: break;
    }
}

void AudioCallback(float *in, float *out, size_t size)
{
    Controls();

    //audio
    for(size_t i = 0; i < size; i += 2)
    {
        float sigl = in[i];
        float sigr = in[i + 1];

        for(int eff = 0; eff < REV; eff++)
        {
            float oldSigL = sigl;
            float oldSigR = sigr;

            if(effectOn[eff])
            {
                GetSample(sigl, sigr, (effectTypes)eff);
            }

            sigl = sigl * dryWet[eff] + oldSigL * (1 - dryWet[eff]);
            sigr = sigr * dryWet[eff] + oldSigR * (1 - dryWet[eff]);
        }

        float verbl = sigl * dryWet[REV];
        float verbr = sigr * dryWet[REV];
        GetReverbSample(verbl, verbr);

        out[i]     = sigl * dryWet[ALL] + in[i] * (1 - dryWet[ALL]);
        out[i + 1] = sigr * dryWet[ALL] + in[i + 1] * (1 - dryWet[ALL]);

        if(effectOn[REV])
        {
            out[i] += verbl;
            out[i + 1] += verbr;
        }
    }
}

int main(void)
{
    // initialize petal hardware and oscillator daisysp module
    float sample_rate;

    //Inits and sample rate
    sp.Init();
    sample_rate = sp.AudioSampleRate();
    rev.Init(sample_rate);
    dell.Init();
    delr.Init();

    //set parameters
    reverbLpParam.Init(sp.knob[0], 400, 22000, Parameter::LOGARITHMIC);
    deltime.Init(sp.knob[1], sample_rate * .05, MAX_DELAY, deltime.LOGARITHMIC);
    crushrate.Init(sp.knob[2], 1, 50, crushrate.LOGARITHMIC);

    for (uint8_t i=0; i < 4; i++)
    {
        sp.encoder[i].SetRange(0, 20);
        sp.encoder[i].SetWrapMode(false);
        sp.encoder[i].SetValue(12);
    }

    //reverb parameters
    rev.SetLpFreq(18000.0f);
    rev.SetFeedback(0.85f);

    //delay parameters
    currentDelay = delayTarget = sample_rate * 0.75f;
    dell.SetDelay(currentDelay);
    delr.SetDelay(currentDelay);

    wah[0].Init(sample_rate);
    wah[1].Init(sample_rate);
    wah[0].SetLevel(.8f);
    wah[1].SetLevel(.8f);

    effectOn[0] = effectOn[1] = effectOn[2] = effectOn[3] = false;

    for(int i = 0; i < LAST; i++)
    {
        dryWet[i] = 0.6f;
    }

    dryWetMode = CRUSH;

    // start callback
    sp.StartAdc();
    sp.StartAudio(AudioCallback);

    while(1)
    {
        UpdateLeds();
        dsy_system_delay(6);
    }
}

void UpdateKnobs()
{
    rev.SetLpFreq(reverbLpParam.Process());
    rev.SetFeedback(sp.knob[4].Process());

    delayTarget = deltime.Process();
    feedback    = sp.knob[5].Process();

    crushmod = (int)crushrate.Process();

    wah[0].SetWah(sp.knob[3].Process());
    wah[1].SetWah(sp.knob[3].Process());
    wah[0].SetLevel(sp.knob[7].Process());
    wah[1].SetLevel(sp.knob[7].Process());

    dryWet[ALL] = sp.knob[8].Process();
}

void UpdateEncoders()
{
    //turn
    dryWet[REV] = 0.05f * sp.encoder[0].Value();
    dryWet[DEL] = 0.05f * sp.encoder[1].Value();
    dryWet[CRUSH] = 0.05f * sp.encoder[2].Value();
    dryWet[WAH] = 0.05f * sp.encoder[3].Value();
}

void UpdateLeds()
{
    //footswitch leds
    sp.SetFootswitchLed(0, effectOn[REV]);
    sp.SetFootswitchLed(1, effectOn[DEL]);
    sp.SetFootswitchLed(2, effectOn[CRUSH]);
    sp.SetFootswitchLed(3, effectOn[WAH]);

    for (uint8_t i = 0; i < 4; i++)
    {
        float val;
        switch (i)
        {
        case 0:
            val = dryWet[REV];
            break;
        case 1:
            val = dryWet[DEL];
            break;
        case 2:
            val = dryWet[CRUSH];
            break;
        case 3:
            val = dryWet[WAH];
            break; 
        }

        if (val == 0.0f)
        {
            sp.SetEncoderLed(i, 0, 0, 0);
        }
        else if (val <= 0.1f)
        {
            sp.SetEncoderLed(i, 0, 0, 1);
        }
        else if (val <= 0.2f)
        {
            sp.SetEncoderLed(i, 0, 1, 0);
        }
        else if (val <= 0.4f)
        {
            sp.SetEncoderLed(i, 0, 1, 1);
        }
        else if (val <= 0.6f)
        {
            sp.SetEncoderLed(i, 1, 1, 1);
        }
        else if (val <= 0.8f)
        {
            sp.SetEncoderLed(i, 1, 1, 0);
        }
        else if (val <= 0.9f)
        {
            sp.SetEncoderLed(i, 1, 0, 1);
        }
        else
        {
            sp.SetEncoderLed(i, 1, 0, 0);
        }
    }

    sp.UpdateLeds();
}

void UpdateSwitches()
{
    //turn the effect on or off if a footswitch is pressed

    effectOn[REV]
        = sp.switches[0].EitherEdge() ? !effectOn[REV] : effectOn[REV];
    effectOn[DEL]
        = sp.switches[1].EitherEdge() ? !effectOn[DEL] : effectOn[DEL];
    effectOn[CRUSH]
        = sp.switches[2].EitherEdge() ? !effectOn[CRUSH] : effectOn[CRUSH];
    effectOn[WAH]
        = sp.switches[3].EitherEdge() ? !effectOn[WAH] : effectOn[WAH];
}

void Controls()
{
    sp.ProcessAnalogControls();
    sp.ProcessDigitalControls();

    UpdateKnobs();
    UpdateEncoders();
    UpdateSwitches();
}

void GetReverbSample(float &inl, float &inr)
{
    rev.Process(inl, inr, &inl, &inr);
}

void GetDelaySample(float &inl, float &inr)
{
    fonepole(currentDelay, delayTarget, .00007f);
    delr.SetDelay(currentDelay);
    dell.SetDelay(currentDelay);
    float outl = dell.Read();
    float outr = delr.Read();

    dell.Write((feedback * outl) + inl);
    inl = (feedback * outl) + ((1.0f - feedback) * inl);

    delr.Write((feedback * outr) + inr);
    inr = (feedback * outr) + ((1.0f - feedback) * inr);
}

void GetCrushSample(float &inl, float &inr)
{
    crushcount++;
    crushcount %= crushmod;
    if(crushcount == 0)
    {
        crushr = inr;
        crushl = inl;
    }

    inr = crushr;
    inl = crushl;
}

void GetWahSample(float &inl, float &inr)
{
    inl = wah[0].Process(inl);
    inr = wah[1].Process(inr);
}