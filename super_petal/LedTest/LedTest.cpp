// Basic Template for SuperPetal
#include "super_petal.h"

using namespace daisy;

SuperPetal hw;
int switch_state[4];

void AudioCallback(float *in, float *out, size_t size)
{
    hw.ProcessDigitalControls();
    hw.ProcessAnalogControls();

    for (uint8_t i = 0; i < 4; i++)
    {
        if (hw.switches[i].EitherEdge() ) {
            switch_state[i]++;
        }
    }    

    for(size_t i = 0; i < size; i += 2)
    {
        out[i]     = in[i];
        out[i + 1] = in[i + 1];
    }
}

int main(void)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        switch_state[i] = 0;
    }

    hw.Init();
    hw.StartAdc();
    hw.StartAudio(AudioCallback);

    hw.lcd.clear();
    hw.lcd.setCursor(0, 0);
    char message[256];
    sprintf(message, "Yay!");
    hw.lcd.print(message);

    while(1) {
        hw.lcd.setCursor(0, 0);
        char message[256];
        sprintf(message, "Yay!");
        hw.lcd.print(message);

        for (uint8_t i = 0; i < 4; i++)
        {
            uint8_t state = switch_state[i] % 8;
            //hw.led_controller.Set(i, state & 1, (state >> 1) & 1, (state >> 2) & 1);
            hw.led_controller.SetValue(i, state);
            hw.led_controller.SetValue(i + 4, hw.encoder[i].Value());
        }
        hw.led_controller.Update();
        dsy_system_delay(6);
    }
}
