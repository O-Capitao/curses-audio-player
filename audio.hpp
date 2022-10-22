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
#define FFT_NUMBER_OF_BANDS (10)

// about FFTs:
// https://www.dataq.com/data-acquisition/general-education-tutorials/fft-fast-fourier-transform-waveform-analysis.html
namespace CursesAudioPlayer {
    
    // PortAudio Callback
    // User obj
    struct InternalAudioData {

        SNDFILE* file = NULL;
        SF_INFO  info;
        
        int buffer_size = FRAMES_PER_BUFFER;
        int readHead = 0;
        int count = 1;
        
        // copy of each buffer, used for output
        float fft_in[FRAMES_PER_BUFFER * NUMBER_OF_CHANNELS];

    };


    // for outbound communication
    // from the engine to some client
    struct ExternalAudioData {
        std::string filename;
        std::string extension;
        float *dataSnapshot;

        std::string stringify();
    };

    /*
    Class for handling basic audio functions
    */
    class AudioEngine {

        private:

            InternalAudioData* _data = NULL;
            PaStream *stream;

            fftwf_plan _fft_plan;
            int band_intensities[FFT_NUMBER_OF_BANDS];
            fftwf_complex _fft_result[FFT_NUMBER_OF_BANDS];
            float _fft_aux_values[FRAMES_PER_BUFFER];
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
            static void _copyArray(float *src, float *tgt, int l);
            static void _copyChannelWithWindowing( float *src, float *tgt, float *window, int total_l, int channel );

        public:

            AudioEngine();
            ~AudioEngine();

            // Player Actions
            void loadFile(const std::string& path);
            void playFile();
            void pauseFile();
            void closeFile();

            // Data access
            // const SoundFileInfo &getSoundFileInfo();
            const ExternalAudioData getAudioData();
            void getFrqDomainData();

    };
}