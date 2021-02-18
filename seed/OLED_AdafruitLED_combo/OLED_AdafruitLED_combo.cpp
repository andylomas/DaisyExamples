#include <stdio.h>
#include <string.h>
#include "daisy_seed.h"
#include "daisysp.h"
#include "myUtils/AdafruitLedController.h"

using namespace daisy;
using namespace daisysp;

DaisySeed   hw;
OledDisplay oled_display;
AdafruitLedController led_controller;
Oscillator osc;

dsy_gpio audio_callback_indicactor;

void AudioCallback(float* in, float* out, size_t size)
{
    dsy_gpio_write(&audio_callback_indicactor, true);

    float x = 0;
    for (int i = 0; i < 2000; i++)
    {
        x *= sinf(i);
    }

    //Fill the block with samples
    for(size_t i = 0; i < size; i += 2)
    {
        float osc_out = x + osc.Process();

        //Set the left and right outputs
        out[i]     = osc_out;
        out[i + 1] = osc_out;
    }

    dsy_gpio_write(&audio_callback_indicactor, false);
}

int main(void)
{   
    // Configure and Initialize the Daisy Seed
    hw.Configure();
    hw.Init();

    //How many samples we'll output per second
    float samplerate = hw.AudioSampleRate();

    // Set up audio callback indicator pin
    audio_callback_indicactor.pin = hw.GetPin(0);
    audio_callback_indicactor.mode = DSY_GPIO_MODE_OUTPUT_PP;
    dsy_gpio_init(&audio_callback_indicactor);

    //Set up oscillator
    osc.Init(samplerate);
    osc.SetWaveform(osc.WAVE_SIN);
    osc.SetAmp(1.f);
    osc.SetFreq(1000);


    // Initialize the OLED display

    // GPIO pins for OLED:
    // 6: CS (if using software CS)
    // 7: CS (if using hardware CS)
    // 8: SCL
    // 9: DC
    // 10: SDA
    // 11: RES

    // Note using software CS:
    //      Connect CS on the OLED to GND
    //      Combine SCL with CS using AND gate and pass to SCL 

    dsy_gpio_pin oled_display_pins[OledDisplay::NUM_PINS];

    oled_display_pins[OledDisplay::DATA_COMMAND] = hw.GetPin(9);
    oled_display_pins[OledDisplay::RESET]        = hw.GetPin(6);

    //dsy_gpio oled_display_cs;
    //oled_display_cs.pin = hw.GetPin(7);
    //oled_display_cs.mode = DSY_GPIO_MODE_OUTPUT_PP;
    //dsy_gpio_init(&oled_display_cs);

    dsy_system_delay(1000);
    
    //dsy_gpio_write(&oled_display_cs, true);
    oled_display.Init(oled_display_pins, true);
    //oled_display.InitSmartUpdate();
    //dsy_gpio_write(&oled_display_cs, false);

    // Initialize the Adafruit LED Controller
    dsy_gpio_pin led_clock_pin = hw.GetPin(11);
    dsy_gpio_pin led_data_pin = hw.GetPin(12); 
    led_controller.Init(led_data_pin, led_clock_pin);
    //led_controller.InitSpi();
    led_controller.SetGlobalBrightness(1.0, 0.35, 0.3);

    // dsy_gpio led_controller_cs;
    // led_controller_cs.pin = hw.GetPin(6);
    // led_controller_cs.mode = DSY_GPIO_MODE_OUTPUT_PP;
    // dsy_gpio_init(&led_controller_cs);
    // dsy_gpio_write(&led_controller_cs, false);

    led_controller.Clear();

    //Start the adc
    //hw.adc.Start();

    //Start calling the audio callback
    hw.StartAudio(AudioCallback);

    // Loop forever
    char strbuff[128];
    int count = 0;
    int val = 0;
    float phase = 0.0f;
    while(1)
    {
        // Set the onboard LED
        hw.SetLed(count%2);

        // Adafruit LED Controller
        float r = 0.5f + 0.5f * sinf(phase);
        float g = 0.5f + 0.5f * sinf(phase + 2.094f);
        float b = 0.5f + 0.5f * sinf(phase - 2.094f);

        for (int i = 0; i < 4; i ++)
        {
            //led_controller.SetFloat(i, r, g, b);
            //led_controller.SetFloat(i, 0.5, 0.5, 0.5);
            led_controller.SetValue(i, val);
            //led_controller.Set(i, 1, 0, 0);
        }

        // led_controller.SetFloat(0, r, g, b);
        // led_controller.SetFloat(1, b, r, g);
        // led_controller.SetFloat(2, g, b, r);
        // led_controller.SetFloat(3, r, g, b);
        
        //dsy_gpio_write(&led_controller_cs, true);
        led_controller.Update();
        //dsy_gpio_write(&led_controller_cs, false);

        phase += 0.01f;
        val++;

        // OLED display
        oled_display.Fill(false);
        oled_display.DrawCircle(64, 64, count % 32, true);
        oled_display.DrawCircle(64, 64, count % 32 + 32, true);
        oled_display.DrawCircle(64, 64, count % 32 + 64, true);

        sprintf(strbuff, "%d", count);
        oled_display.SetCursor(0, 0);
        oled_display.WriteString(strbuff, Font_7x10, true);
        count++;

        //dsy_gpio_write(&oled_display_cs, true);

        // if (val % 32 == 0)
        // {
        //     oled_display.Init(oled_display_pins, false);
        // }

        oled_display.Update();
        //dsy_gpio_write(&oled_display_cs, false);


        // Wait 20ms
        dsy_system_delay(50);
    }
}
