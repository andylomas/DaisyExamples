#ifndef FMOPERATOR_H
#define FMOPERATOR_H

#include "daisy_core.h"
#include "math.h"

class FMOperator
{
public:

    enum
    {
        WAVE_SIN,
        WAVE_BHASKARA_SIN,
        WAVE_TRI,
        WAVE_SAW,
        WAVE_RAMP,
        WAVE_SQUARE
     };

    void Init(const float sample_rate);
    float Process(float phase_offset);
    inline void SetFeedback(const bool v=true) { use_feedback_ = v; }
    inline void SetFreq(const float f) { inc_ = f * sample_rate_recip_; }
    inline void SetAmp(const float a) { amp_ = a; }
    inline void SetWaveform(const uint8_t wf) { waveform_ = wf; }
    inline uint8_t Waveform() { return waveform_; }

private:
    float RawWaveformOutput(const float phase_offset);

    float sample_rate_recip_;
    int waveform_;
    float inc_;
    float amp_;
    float phase_;
    float rel_freq_;
    bool rel_mode_;
    bool use_feedback_;
};

#endif
