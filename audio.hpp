#pragma once

#include "portaudio.h"
#include "sndfile.hh"
#include <string>

// In particular, if you #include <complex.h> before <fftw3.h>,
// then fftw_complex is defined to be the native complex type 
// and you can manipulate it with ordinary arithmetic (e.g. x = y * (3+4*I), 
// where x and y are fftw_complex and I is the standard symbol for the imaginary unit)
#include <complex>
#include <fftw3.h>

#define FRAMES_PER_BUFFER (512)
#define NUMBER_OF_CHANNELS (2)

// about FFTs:
// https://www.dataq.com/data-acquisition/general-education-tutorials/fft-fast-fourier-transform-waveform-analysis.html
namespace CursesAudioPlayer {

    struct SoundFileInfo {
        std::string extension,
        flename,
        path;

        int samplerate, n_channels;
    };

    struct SpectrumInfoFrame {

        int lowerRange;
        int upperRange;
        int n_slices;
        std::complex<float> freqs[];

    };

    struct AudioData {
        SNDFILE* file = NULL;
        SF_INFO  info;
        SoundFileInfo sfInfo;
        int buffer_size = 512;
        int readHead = 0;
        int count = 1;

        double fft_in[FRAMES_PER_BUFFER * NUMBER_OF_CHANNELS];
        fftw_complex fft_out[FRAMES_PER_BUFFER * NUMBER_OF_CHANNELS];
        fftw_plan plan;
    };

    /*
    Class for handling basic audio functions
    */
    class AudioEngine {

        private:

            AudioData* _data = NULL;
            PaStream *stream;
            double _windowFunctionPoints[FRAMES_PER_BUFFER];

            bool PLAYING = false;

            void _unloadFile();

            static int _paStreamCallback( 
                const void *inputBuffer,
                void *outputBuffer,
                unsigned long framesPerBuffer,
                const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags,
                void *userData 
            );
            
            // https://en.wikipedia.org/wiki/Hann_function
            void _calculateWindowFunction();
            static void _multiplyArrays(double *arr1, double*arr2, double *out, int l);
            static void _copyArray(float *src, double*tgt, int l);

        public:

            AudioEngine();
            ~AudioEngine();
            
            void loadFile(const std::string& path);
            
            void playFile();
            void pauseFile();
            void closeFile();

            const SoundFileInfo &getSoundFileInfo();

            void getFrqDomainData();
            

    };
}