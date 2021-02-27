#include "daisysp.h"
#include "super_patch.h"
#include <stdio.h>
#include <string.h>

using namespace daisy;
using namespace daisysp;

SuperPatch    sp;

// Main -- Init
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
    //sp.uart.Init(SERIAL_USART1, 3686400); // Highest baud rate for tx on USART1
    sp.uart.Init(SERIAL_USART1, 6250000); // Highest baud rate for tx on USART1 using 16x oversampling
    //sp.uart.Init(SERIAL_USART1, 12500000); // Highest baud rate for tx on USART1 using 8x oversampling

    bool led_state;
    led_state = true;

    while(1)
    {
        // Toggle the LED state for the next time around.
        led_state = !led_state;

        // Set the onboard LED
        sp.seed.SetLed(led_state);

        char message[256];
        sprintf(message, "ABCDEFGH\n");
        sp.uart.PollTx((uint8_t*)message, 8);

        // Wait 200ms
        dsy_system_delay(200);
    }
}
