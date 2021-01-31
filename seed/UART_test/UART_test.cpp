#include "daisy_pod.h"
#include "daisysp.h"
#include "myUtils/myUart.h"
#include <stdio.h>
#include <string.h>

using namespace daisy;
using namespace daisysp;

DaisySeed    hw;
MyUartHandler  uart;

//MidiHandler midi;
//Oscillator  osc;
//Svf         filt;

// void AudioCallback(float *in, float *out, size_t size)
// {
//     float sig;
//     for(size_t i = 0; i < size; i += 2)
//     {
//         sig = osc.Process();
//         filt.Process(sig);
//         out[i] = out[i + 1] = filt.Low();
//     }
// }

// // Typical Switch case for Message Type.
// void HandleMidiMessage(MidiEvent m)
// {
//     switch(m.type)
//     {
//         case NoteOn:
//         {
//             NoteOnEvent p = m.AsNoteOn();
//             char        buff[512];
//             sprintf(buff,
//                     "Note Received:\t%d\t%d\t%d\r\n",
//                     m.channel,
//                     m.data[0],
//                     m.data[1]);
//             hw.seed.usb_handle.TransmitInternal((uint8_t *)buff, strlen(buff));
//             // This is to avoid Max/MSP Note outs for now..
//             if(m.data[1] != 0)
//             {
//                 p = m.AsNoteOn();
//                 osc.SetFreq(mtof(p.note));
//                 osc.SetAmp((p.velocity / 127.0f));
//             }
//         }
//         break;
//         case ControlChange:
//         {
//             ControlChangeEvent p = m.AsControlChange();
//             switch(p.control_number)
//             {
//                 case 1:
//                     // CC 1 for cutoff.
//                     filt.SetFreq(mtof((float)p.value));
//                     break;
//                 case 2:
//                     // CC 2 for res.
//                     filt.SetRes(((float)p.value / 127.0f));
//                     break;
//                 default: break;
//             }
//             break;
//         }
//         default: break;
//     }
// }


// Main -- Init, and Midi Handling
int main(void)
{
    // Init
    float samplerate;
    hw.Configure();
    hw.Init();
    hw.usb_handle.Init(UsbHandle::FS_INTERNAL);

    dsy_system_delay(250);
    //uart.Init(SERIAL_USART1, 115200);
    //uart.Init(SERIAL_USART1, 230400);
    //uart.Init(SERIAL_USART1, 230400);
    //uart.Init(SERIAL_USART1, 460800);
    //uart.Init(SERIAL_USART1, 921600);
    //uart.Init(SERIAL_USART1, 1843200);
    uart.Init(SERIAL_USART1, 3686400);
    //uart.Init(SERIAL_USART1, 7372800);

    bool led_state;
    led_state = true;

    for(;;)
    {
        // Toggle the LED state for the next time around.
        led_state = !led_state;

        // Set the onboard LED
        hw.SetLed(led_state);

        char message[256];
        sprintf(message, "ABCDEFGH\n");
        uart.PollTx((uint8_t*)message, 8);

        // Wait 200ms
        dsy_system_delay(200);
    }
}
