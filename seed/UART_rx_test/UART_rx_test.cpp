#include "daisy_pod.h"
#include "daisysp.h"
#include "myUtils/myUart.h"
#include <stdio.h>
#include <string.h>

using namespace daisy;
using namespace daisysp;

DaisySeed    hw;
MyUartHandler  uart;


int main(void)
{
    // Init
    hw.Configure();
    hw.Init();

    dsy_system_delay(250);
    //uart.Init(SERIAL_USART1, 115200);
    //uart.Init(SERIAL_USART1, 230400);
    //uart.Init(SERIAL_USART1, 230400);
    //uart.Init(SERIAL_USART1, 460800);
    uart.Init(SERIAL_USART1, 921600); // Highest standard baud rate for PicoScript 
    //uart.Init(SERIAL_USART1, 1843200);
    //uart.Init(SERIAL_USART1, 3686400); // Highest baud rate for tx on USART1


    bool led_state;
    led_state = true;
    uart.StartRx();
 
    for(;;)
    {
        // uint8_t message[256];
        // uint8_t ret = uart.PollReceive(message, 8, 10);

        // if (ret != 2)
        // {
        //     uart.PollTx(&ret, 1);
        // }
        
        //if (ret == 32)
        //{
        //    uart.PollTx(message, 8);
        //}

        //uart.PollTx(&ret, 1);

        //uart.PollTx(message, 8);

        if (uart.Readable())
        {
            led_state = !led_state;
            hw.SetLed(led_state);

            uint8_t val = uart.PopRx();
            uart.PollTx(&val, 1);
        }

        // if(!uart.RxActive())
        // {
        //     uart.FlushRx();
        //     uart.StartRx();
        // }

        //led_state = !led_state;
        //hw.SetLed(led_state);

        // Wait 200ms
        //dsy_system_delay(500);
    }
}
