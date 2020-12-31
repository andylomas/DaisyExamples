#include "FMSynth.h"

void FMSynth::Init(const float sample_rate)
{
    // Initialize all the operators
    for (int i = 0; i < NUM_OPERATORS; i ++)
    {
        op[i].Init(sample_rate);
        sig_[i] = 0.0f;
    }

    // Initialize the operator connection topology to 0
    SetOperatorTopology(0);
}

float FMSynth::Process()
{
    // We only need to set sig[NUM_OPERATORS] to zero since all the other signal values
    // should already have been set to zero in the previous iteration
    // Set signal level of all operators, including the output operator, to zero
    for (int i = 0; i <= NUM_OPERATORS; i++)
    {
        sig_[i] = 0.0f;
    }
  
    // Calculate the output of each operator and add to the relevant output
    // Note: we count down through the operators as they always connect to alower
    // index operator or the final output.
    for (int i = NUM_OPERATORS - 1 ; i >= 0; i--)
    {
        sig_[output_connection_[i]] += op[i].Process(sig_[i]);
    }

    // The final signal in the array of signal values is the final result
    return sig_[NUM_OPERATORS];
}

void FMSynth::SetOperatorTopology(const uint8_t topology_id)
{
    // Note: output number 4 is the final signal output of the synth
    switch (topology_id)
    {
    case 0:
        output_connection_[0] = 4;
        output_connection_[1] = 0;
        output_connection_[2] = 1;
        output_connection_[3] = 2;
        break;

    case 1:
        output_connection_[0] = 4;
        output_connection_[1] = 0;
        output_connection_[2] = 1;
        output_connection_[3] = 1;
        break;

    case 2:
        output_connection_[0] = 4;
        output_connection_[1] = 0;
        output_connection_[2] = 1;
        output_connection_[3] = 0;
        break;

    case 3:
        output_connection_[0] = 4;
        output_connection_[1] = 0;
        output_connection_[2] = 0;
        output_connection_[3] = 0;
        break;

    case 4:
        output_connection_[0] = 4;
        output_connection_[1] = 0;
        output_connection_[2] = 1;
        output_connection_[3] = 4;
        break;

    case 5:
        output_connection_[0] = 4;
        output_connection_[1] = 0;
        output_connection_[2] = 0;
        output_connection_[3] = 4;
        break;

    case 6:
        output_connection_[0] = 4;
        output_connection_[1] = 0;
        output_connection_[2] = 4;
        output_connection_[3] = 2;
        break;

    case 7:
        output_connection_[0] = 4;
        output_connection_[1] = 0;
        output_connection_[2] = 4;
        output_connection_[3] = 4;
        break;

    case 8:
        output_connection_[0] = 4;
        output_connection_[1] = 4;
        output_connection_[2] = 4;
        output_connection_[3] = 4;
        break;
    }
}
