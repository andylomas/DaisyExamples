#include "daisysp.h"
#include "super_patch.h"

using namespace daisysp;
using namespace daisy;

#define NUM_WAVEFORMS 8

SuperPatch sp;
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
    for (int i = 0; i < 8; i++)
    {
        basenote += 4 * (i + 1) * sp.knob[i].Value();
    }

    // Test banana analog output
    static float dacVal1 = 0.f;
    static float dacVal2 = 0.f;
    dacVal1 += 0.00025f;
    dacVal2 += 0.004f;
    if (dacVal1 > 1.0f) dacVal1 = 0.f;
    if (dacVal2 > 1.0f) dacVal2 = 0.f;
    sp.banana[0].SetValue(dacVal1);
    sp.banana[2].SetValue(dacVal2);

    // Test banana analog input
    basenote += 4 * sp.banana[1].Value() + 8 * sp.banana[3].Value();

    osc.SetFreq(mtof(basenote + 2 * (sp.button[0].Pressed() + 2 * sp.button[1].Pressed())));

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

    // Configure the Banana plugs
    BananaConfig banana_config[NUM_BANANAS];
    banana_config[0].mode = ANALOG_OUTPUT;
    banana_config[1].mode = ANALOG_INPUT;
    banana_config[2].mode = ANALOG_OUTPUT;
    banana_config[3].mode = ANALOG_INPUT;
    banana_config[4].mode = SERIAL_OUTPUT;
    banana_config[4].baud_rate = 115200;
    banana_config[5].mode = SERIAL_INPUT;
    banana_config[5].baud_rate = 115200;

    // Init everything
    sp.Init(banana_config);
    samplerate = sp.AudioSampleRate();

    sp.encoder[0].SetRange(0, 23);
    sp.encoder[1].SetRange(0, 23);

    osc.Init(samplerate);
    osc.SetAmp(1.f);
    osc.SetFreq(500.0f);

    // start callbacks
    sp.StartAdc();
    sp.StartAudio(AudioCallback);

    // Loop forever
    int count = 0;
    char strbuff[128];
    while(1) {
        sp.SetOnboardLed(count % 2);

        // Adafruit LED Controller
        float r0 = sp.knob[4].Value();
        float g0 = sp.knob[0].Value();
        float b0 = (float)sp.encoder[0].Value() / sp.encoder[0].MaxVal();
        if (sp.encoder[0].Pressed()) b0 = 1.0f - b0;

        float r1 = sp.knob[5].Value();
        float g1 = sp.knob[1].Value();
        float b1 = (float)sp.encoder[1].Value() / sp.encoder[1].MaxVal();
        if (sp.encoder[1].Pressed()) b1 = 1.0f - b1;

        float r2 = sp.knob[6].Value();
        float g2 = sp.knob[2].Value();
        float b2 = sp.button[0].Pressed();

        float r3 = sp.knob[7].Value();
        float g3 = sp.knob[3].Value();
        float b3 = sp.button[1].Pressed();

        sp.SetLedFloat(0, r0, g0, b0);
        sp.SetLedFloat(1, r1, g1, b1);
        sp.SetLedFloat(2, r2, g2, b2);
        sp.SetLedFloat(3, r3, g3, b3);

        sp.UpdateLeds();

        // OLED display
        sp.display.Fill(false);
        sp.display.DrawCircle(64, 64, count % 32, true);
        sp.display.DrawCircle(64, 64, count % 32 + 32, true);
        sp.display.DrawCircle(64, 64, count % 32 + 64, true);

        sprintf(strbuff, "%d", count);
        sp.display.SetCursor(0, 0);
        sp.display.WriteString(strbuff, Font_7x10, true);
        count++;

        sprintf(strbuff, "Pot0: %d.%d", (int)(10 * sp.knob[0].Value()) / 100, (int)(100 * sp.knob[0].Value()) % 100);
        sp.display.SetCursor(0, 12);
        sp.display.WriteString(strbuff, Font_7x10, true);
        count++;

        sprintf(strbuff, "Pot1: %d.%d", (int)(10 * sp.knob[1].Value()) / 100, (int)(100 * sp.knob[1].Value()) % 100);
        sp.display.SetCursor(0, 24);
        sp.display.WriteString(strbuff, Font_7x10, true);
        count++;
        
        sprintf(strbuff, "Encoder0: %d", sp.encoder[0].Value());
        sp.display.SetCursor(0, 36);
        sp.display.WriteString(strbuff, Font_7x10, true);
        count++;

        sprintf(strbuff, "Encoder1: %d", sp.encoder[1].Value());
        sp.display.SetCursor(0, 48);
        sp.display.WriteString(strbuff, Font_7x10, true);
        count++;

        sp.display.Update();

        dsy_system_delay(20);
    }
}
