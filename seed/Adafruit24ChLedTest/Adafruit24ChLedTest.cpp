#include "daisy_seed.h"
#include "myUtils/Adafruit24ChLedController.h"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;

// Declare a DaisySeed object called hardware
DaisySeed hardware;
Adafruit24ChLedController led_controller;

int main(void)
{
    // Declare a variable to store the state we want to set for the LED.
    bool led_state;
    led_state = true;

    // Configure and Initialize the Daisy Seed
    // These are separate to allow reconfiguration of any of the internal
    // components before initialization.
    hardware.Configure();
    hardware.Init();

    dsy_gpio_pin led_data_pin = hardware.GetPin(1);
    dsy_gpio_pin led_clock_pin = hardware.GetPin(2);
    dsy_gpio_pin led_latch_pin = hardware.GetPin (3);
    led_controller.Init(led_data_pin, led_clock_pin, led_latch_pin);
    led_controller.SetGlobalBrightness(1.0, 0.25, 0.4);
    led_controller.SetGamma(2.8);

    led_controller.Clear();

    // Loop forever
    float phase = 0.0f;
    int val = 0;
    while(true)
    {
        // Set the onboard LED
        hardware.SetLed(led_state);

        // Toggle the LED state for the next time around.
        led_state = !led_state;

        float r = 0.5f + 0.5f * sinf(phase);
        float g = 0.5f + 0.5f * sinf(phase + 2.094f);
        float b = 0.5f + 0.5f * sinf(phase - 2.094f);

        for (int i = 0; i < 8; i ++)
        {
            led_controller.SetFloat(i, r, g, b);
            //led_controller.SetFloat(i, 0.5, 0.5, 0.5);
            //led_controller.SetValue(i, val);
            //led_controller.Set(i, 1, 1, 1);
            //led_controller.SetRaw(i, 0b101010101010, 0b101010101010, 0b101010101010);
        }
        
        //dsy_gpio_write(&led_cs, true);
        led_controller.Update();
        //dsy_gpio_write(&led_cs, false);

        phase += 0.01f;
        val = (val + 1) % 8;

        // Wait 500ms
        dsy_system_delay(5);
    }
}
