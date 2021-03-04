#include "daisy_seed.h"
#include "daisysp.h"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;
using namespace daisysp;

// Declare a DaisySeed object called hardware
DaisySeed  hardware;
Oscillator osc;
dsy_gpio pin_timing;

#define NUM_LOOKUP_POINTS 1024
float sin_lookup_data[NUM_LOOKUP_POINTS + 1];

void generate_sin_lookup_data()
{
    for (int i = 0; i < NUM_LOOKUP_POINTS + 1; i++)
    {
        float x = TWOPI_F * i / NUM_LOOKUP_POINTS;
        sin_lookup_data[i] = sin(x);
    }
}

float sin_lookup(float x)
{
    int i = (int)(x * NUM_LOOKUP_POINTS / TWOPI_F) % NUM_LOOKUP_POINTS;
    if (i < 0) i += NUM_LOOKUP_POINTS;
    return sin_lookup_data[i];
}

float blended_sin_lookup(float x)
{
    float intpart;
    float f = modf(x * NUM_LOOKUP_POINTS / TWOPI_F, &intpart);
    int i = (((int)intpart % NUM_LOOKUP_POINTS) + NUM_LOOKUP_POINTS) % NUM_LOOKUP_POINTS;
    //f = (f < 0) ? -f : f;
    return (1.0f - f) * sin_lookup_data[i] + f * sin_lookup_data[i + 1];
}

float bhaskara_sin(float x)
{
    float y = x * (PI_F - x);
    return (16.0f * y) / (49.348022f - 4.0f * y);
}

float bhaskara_normalized_sin(float z)
{
    float y = z * (1.f - z);
    return (16.f * y) / (5.f - 4.f * y);
}

float square_wave(float x)
{
    return (x >= 0) ? 1.0f : -1.0f;
}

void AudioCallback(float* in, float* out, size_t size)
{
    float osc_out;

    float x = 1;
    dsy_gpio_write(&pin_timing, 1);
    for (int i = 0; i < 1000; i++)
    {
        x = sin(x);
    }
    dsy_gpio_write(&pin_timing, 0);

    // Fill the block with samples
    for(size_t i = 0; i < size; i += 2)
    {
        osc_out = osc.Process() + x;
        out[i]     = osc_out;
        out[i + 1] = osc_out;
    }
}


int main(void)
{
    // Configure and Initialize the Daisy Seed
    // These are separate to allow reconfiguration of any of the internal
    // components before initialization.
    hardware.Configure();
    hardware.Init();

    // How many samples we'll output per second
    float samplerate = hardware.AudioSampleRate();

    // Setup timing test output pin
    pin_timing.pin = hardware.GetPin(0);
    pin_timing.mode = DSY_GPIO_MODE_OUTPUT_PP;
    dsy_gpio_init(&pin_timing);

    // Set up oscillator
    osc.Init(samplerate);
    osc.SetWaveform(osc.WAVE_SIN);
    osc.SetAmp(1.f);
    osc.SetFreq(1000);

    // Generate lookup data
    generate_sin_lookup_data();

    // Start calling the audio callback
    hardware.StartAudio(AudioCallback);

    // Loop forever
    bool led_state = false;
    for(;;) {
        // Set the onboard LED
        hardware.SetLed(led_state);

        // Toggle the LED state for the next time around.
        led_state = !led_state;

        // Wait 500ms
        dsy_system_delay(500);
    }
}
