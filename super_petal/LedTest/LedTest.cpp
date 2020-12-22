// Basic Template for SuperPetal
#include "super_petal.h"

using namespace daisy;

SuperPetal sp;
int switch_state[4];

void AudioCallback(float *in, float *out, size_t size)
{
    sp.ProcessDigitalControls();
    sp.ProcessAnalogControls();

    for (uint8_t i = 0; i < 4; i++)
    {
        if (sp.switches[i].EitherEdge() ) {
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

    sp.Init();
    sp.StartAdc();
    sp.StartAudio(AudioCallback);

    sp.lcd.clear();
    sp.lcd.setCursor(0, 0);
    char message[256];
    sprintf(message, "Yay! wibble flop");
    sp.lcd.print(message);

    int i = 0;
    while(1) {
        sp.lcd.setCursor(0, 1);
        char message[256];
        sprintf(message, "%d", i);
        sp.lcd.print(message);

        for (uint8_t i = 0; i < 4; i++)
        {
            uint8_t state = switch_state[i] % 8;
            sp.led_controller.SetValue(i, state);
            sp.led_controller.SetValue(i + 4, sp.encoder[i].Value());
        }
        sp.led_controller.Update();
        //dsy_system_delay(6);
        i++;
    }
}
