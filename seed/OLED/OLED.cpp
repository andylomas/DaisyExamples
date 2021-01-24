#include <stdio.h>
#include <string.h>
#include "daisy_seed.h"

using namespace daisy;

DaisySeed   hw;
OledDisplay display;

int main(void)
{
    uint8_t      message_idx;
    dsy_gpio_pin oled_pins[OledDisplay::NUM_PINS];
    hw.Configure();
    hw.Init();

    oled_pins[OledDisplay::DATA_COMMAND] = hw.GetPin(9);
    oled_pins[OledDisplay::RESET]        = hw.GetPin(30);
    display.Init(oled_pins);
    display.InitSmartUpdate();

    message_idx = 0;
    char strbuff[128];
    int i = 0;
    while(1)
    {
        dsy_system_delay(100);
        // switch(message_idx)
        // {
        //     case 0: sprintf(strbuff, "Testing. . ."); break;
        //     case 1: sprintf(strbuff, "Daisy. . ."); break;
        //     case 2: sprintf(strbuff, "1. . ."); break;
        //     case 3: sprintf(strbuff, "2. . ."); break;
        //     case 4: sprintf(strbuff, "3. . ."); break;
        //     default: break;
        // }
        // message_idx = (message_idx + 1) % 5;
        display.Fill(false);
        // display.SetCursor(0, 0);
        // display.WriteString(strbuff, Font_6x8, true);

        // display.SetCursor(0, 9);
        // display.WriteString(strbuff, Font_7x10, true);

        // display.SetCursor(0, 20);
        // display.WriteString(strbuff, Font_11x18, true);

        // display.SetCursor(0, 37);
        // display.WriteString(strbuff, Font_16x26, true);

        display.DrawCircle(64, 64, i % 32 + 1, true);

        sprintf(strbuff, "%d", i);
        display.SetCursor(0, 0);
        display.WriteString(strbuff, Font_7x10, true);
    
        //uint8_t flags = display.CalcBufferChecksums();
        //sprintf(strbuff, "%d", flags);
        //display.SetCursor(0, 12);
        //display.WriteString(strbuff, Font_7x10, true);

        i++;

        display.SmartUpdate();
    }
}
