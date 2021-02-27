#include "daisysp.h"
#include "super_patch.h"
#include <stdio.h>
#include <string.h>

using namespace daisy;
using namespace daisysp;

SuperPatch sp;


int main(void)
{
    // Init
    sp.Init();

    dsy_system_delay(250);
    //sp.uart.Init(SERIAL_USART1, 115200);
    //sp.uart.Init(SERIAL_USART1, 230400);
    //sp.uart.Init(SERIAL_USART1, 230400);
    //sp.uart.Init(SERIAL_USART1, 460800);
    //sp.uart.Init(SERIAL_USART1, 921600); // Highest standard baud rate for PicoScript 
    //sp.uart.Init(SERIAL_USART1, 1843200);
    //sp.uart.Init(SERIAL_USART1, 3686400);
    sp.uart.Init(SERIAL_USART1, 6250000); // Highest baud rate for tx on USART1 using 16x oversampling
    //sp.uart.Init(SERIAL_USART1, 12500000); // Highest baud rate for tx on USART1 using 8x oversampling

    bool led_state;
    led_state = true;
    sp.uart.StartRx();
 
    uint32_t count = 0;
    while(1)
    {
        if (sp.uart.Readable())
        {
            uint8_t val = sp.uart.PopRx();
            sp.uart.PollTx(&val, 1);
            count++;

            if (count % 8 == 0)
            {
                led_state = !led_state;
                sp.seed.SetLed(led_state);
            }
        }

        if(!sp.uart.RxActive())
        {
            sp.uart.FlushRx();
            sp.uart.StartRx();
        }

        // Wait 200ms
        //dsy_system_delay(1);
    }
}
