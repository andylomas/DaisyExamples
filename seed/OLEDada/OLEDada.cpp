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

    oled_pins[OledDisplay::DATA_COMMAND] = hw.GetPin(9);
    oled_pins[OledDisplay::RESET]        = hw.GetPin(11);

    dsy_gpio oled_cs;
    oled_cs.pin = hw.GetPin(6);
    oled_cs.mode = DSY_GPIO_MODE_OUTPUT_PP;
    dsy_gpio_init(&oled_cs);

    // Initialize OLED. true = hardware CS, false = software CS
    dsy_system_delay(1000);
    dsy_gpio_write(&oled_cs, true);
    display.Init(oled_pins, true);
    //display.InitSmartUpdate();
    dsy_gpio_write(&oled_cs, false);


    message_idx = 0;
    char strbuff[128];
    int i = 0;
    while(1)
    {
        dsy_system_delay(100);
        hw.SetLed(i%2);

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
        //display.Fill(false);
        // display.SetCursor(0, 0);
        // display.WriteString(strbuff, Font_6x8, true);

        // display.SetCursor(0, 9);
        // display.WriteString(strbuff, Font_7x10, true);

        // display.SetCursor(0, 20);
        // display.WriteString(strbuff, Font_11x18, true);

        // display.SetCursor(0, 37);
        // display.WriteString(strbuff, Font_16x26, true);

        display.DrawCircle(64, 64, i % 128 + 1, true);

        sprintf(strbuff, "%d", i);
        display.SetCursor(0, 0);
        display.WriteString(strbuff, Font_7x10, true);
    
        //uint8_t flags = display.CalcBufferChecksums();
        //sprintf(strbuff, "%d", flags);
        //display.SetCursor(0, 12);
        //display.WriteString(strbuff, Font_7x10, true);

        i++;
        //display.Fill(i%2);

        dsy_gpio_write(&oled_cs, true);
        //display.SmartUpdate();
        display.Update();
        dsy_gpio_write(&oled_cs, false);
    }
}
