#pragma once

#include "portaudio.h"
#include "sndfile.hh"
#include <string>

namespace CursesAudioPlayer {

    struct SoundFileInfo {
        std::string extension,
        flename,
        path;

        int samplerate, n_channels;
    };

    struct AudioFile {
        SNDFILE* file = NULL;
        SF_INFO  info;
        SoundFileInfo sfInfo;
        int      buffer_size = 512;
        int      readHead = 0;
        int      count = 1;
    };

    /*
    Class for handling basic audio functions
    */
    class AudioEngine {

        private:

            AudioFile* activeFile = NULL;
            PaStream *stream;

            void _unloadFile();

            static int _paStreamCallback( 
                const void *inputBuffer,
                void *outputBuffer,
                unsigned long framesPerBuffer,
                const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags,
                void *userData 
            ) ;

        public:

            AudioEngine();
            ~AudioEngine();
            void loadFile(const std::string& path);
            void playFile();
            const SoundFileInfo &getSoundFileInfo();
    };
}