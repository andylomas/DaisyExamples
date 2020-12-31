#ifndef FMSYNTH_H
#define FMSYNTH_H

#include "daisy_core.h"
#include "FMOperator.h"

#define NUM_OPERATORS 4
#define NUM_OPERATOR_WAVEFORMS 5
#define NUM_OPERATORS_PLUS_ONE 5
#define NUM_OPERATOR_TOPOLOGIES 9

// Operators are referenced through numbers 0 to 3
// sig_[4] is used to reference the final output signal

class FMSynth
{
public:
    void Init(const float sample_rate);
    float Process(); 
    void SetOperatorTopology(const uint8_t topology_id);
    inline void SetOperatorFeedback(const uint8_t n, const bool v=true) { op[n].SetFeedback(v); }

public:
    FMOperator op[NUM_OPERATORS];

private:
    int output_connection_[NUM_OPERATORS];
    float sig_[NUM_OPERATORS_PLUS_ONE];
};

#endif
