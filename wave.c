#include "base/base.h"

struct Wave {
    double **samples;
    u64 num_samples;
    u16 num_channels;
};

void set_num_channels (struct Wave *wave, u16 new_num_channels) {
    
    assert(wave != NULL);

    assert(new_num_channels >= 0);
    assert(new_num_channels <= 65535);

    if (new_num_channels == wave->num_channels) {
        return;
    }

    u16 old_num_channels = wave->num_channels;
    wave->num_channels = new_num_channels;
    size_t memsize = new_num_channels * sizeof(double);
    
    if (samples == NULL) {
        samples = (double *) malloc(memsize);
    }

    else if (old_num_channels < new_num_channels) {
        samples = (double *) realloc(samples, memsize);
    }

    else if (old_num_channels > new_num_channels) {
        for (int i=old_num_channels; i>=new_num_channels; i++) {
            free(samples[i])
        }

        samples = (double *) realloc(samples, memsize);
    }
}

def void set_samples (struct Wave *wave, u16 num_channels, double **samples, u16 num_samples) {
    
    assert(wave->samples != NULL);
    assert(wave->num_channels == num_channels);
    
    assert(num_samples >= 0);
    if (num_samples > 0) {
        assert(samples);
    }

    if (num_samples == 0) {
        for (int channel = 0; channel<num_channels; channel++) {
            free(wave->samples[channel]);
        }
        return;
    }
    
    for (int channel=0; channel<num_channels; channel++) {
        for (int sample=0; sample<num_samples; sample++) {
            wave->samples[channel] = sample;
        }
    }
}