#include "FMOperator.h"

#define PI_F 3.1415927f
#define PI_RECIP_F 0.3183098f
#define TAU_F 6.2831853f
#define TAU_RECIP_F 0.1591549f

#define frac(f) f - floor(f)

void FMOperator::Init(const float sample_rate)
{
    sample_rate_recip_ = 1.0f / sample_rate;
    waveform_ = WAVE_SIN;
    amp_ = 1.0f;
    phase_ = 0.0f;
    inc_ = 0.0f;
}

float FMOperator::RawWaveformOutput(const float phase_offset)
{
    float out, t, p;
    
    switch(waveform_)
    {
        case WAVE_SIN:
            out = sinf(TAU_F * (phase_ + phase_offset));
            break;
            
        case WAVE_TRI:
            p = frac(phase_ + phase_offset);
            t = -1.0f + 2.0f * p;
            out = 2.0f * (fabsf(t) - 0.5f);
            break;
            
        case WAVE_SAW:
            p = frac(phase_ + phase_offset);
            out = -1.0f * (2.0f * p - 1.0f);
            break;
            
        case WAVE_RAMP:
            p = frac(phase_ + phase_offset);
            out = 2.0f * p - 1.0f;
            break;
          
        case WAVE_SQUARE:
            p = frac(phase_ + phase_offset);
            out = (p < 0.5) ? 1.0f : -1.0f;
            break;
          
        default: out = 0.0f; break;
    }

    return amp_ * out;
}

float FMOperator::Process(float phase_offset)
{
    float out;

    if (use_feedback_)
    {
        phase_offset += RawWaveformOutput(phase_offset);
    }

    out = RawWaveformOutput(phase_offset);

    // Increment phase_, setting back into range 0-1 if phase_ > 1
    phase_ += inc_ - floor(phase_);

    return out;
}
