#include "daisy_pod.h"
#include "daisysp.h"
#include <stdio.h>
#include <string.h>

using namespace daisy;
using namespace daisysp;

DaisySeed    hw;
MidiHandler midi;
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
    midi.Init(MidiHandler::INPUT_MODE_UART1, MidiHandler::OUTPUT_MODE_UART1);

    bool led_state;
    led_state = true;

    dsy_gpio out_pin0;
    out_pin0.pin  = hw.GetPin(28);
    out_pin0.mode = DSY_GPIO_MODE_OUTPUT_PP;
    dsy_gpio_init(&out_pin0);

    dsy_gpio out_pin1;
    out_pin1.pin  = hw.GetPin(27);
    out_pin1.mode = DSY_GPIO_MODE_OUTPUT_PP;
    dsy_gpio_init(&out_pin1);

    dsy_gpio out_pin2;
    out_pin2.pin  = hw.GetPin(26);
    out_pin2.mode = DSY_GPIO_MODE_OUTPUT_PP;
    dsy_gpio_init(&out_pin2);

    // Synthesis
    //samplerate = hw.AudioSampleRate();
    //osc.Init(samplerate);
    //osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    //filt.Init(samplerate);

    // Start stuff.
    //hw.StartAudio(AudioCallback);
    //midi.StartReceive();
    for(;;)
    {
        // Toggle the LED state for the next time around.
        led_state = !led_state;

        // Set the onboard LED
        hw.SetLed(led_state);

        dsy_gpio_write(&out_pin0, midi.last_error_state % 2);
        dsy_gpio_write(&out_pin1, midi.last_error_state / 2);
        dsy_gpio_write(&out_pin2, led_state);

        // midi.Listen();
        // // Handle MIDI Events
        // while(midi.HasEvents())
        // {
        //     HandleMidiMessage(midi.PopEvent());
        // }

        char message[256];
        sprintf(message, "ABCDEFGH\n");
        midi.SendMessage((uint8_t*)message, 8);

        // Wait 200ms
        dsy_system_delay(200);
    }
}
