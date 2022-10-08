#include "audio.hpp"
#include <string>
#include  <filesystem>

#define FRAMES_PER_BUFFER   (512)

using namespace CursesAudioPlayer;

AudioEngine::AudioEngine(){

}

AudioEngine::~AudioEngine(){
    _unloadFile();
}


void AudioEngine::loadFile( const std::string& path){

    if (activeFile != NULL){
        _unloadFile();
    }

    activeFile = new AudioFile();
    activeFile->file = sf_open( path.c_str(), SFM_READ, &activeFile->info);

    playFile();

}

void AudioEngine::_unloadFile(){

    if (activeFile == NULL) return;

    sf_close(activeFile->file);
    delete activeFile->file;

    delete activeFile;

    activeFile = NULL;

}

const SoundFileInfo &AudioEngine::getSoundFileInfo(){
    if (activeFile != NULL){
        return activeFile->sfInfo;
    } 
    throw std::runtime_error("No active File, dumping...\n");
}

int AudioEngine::_paStreamCallback(const void                     *input
    ,void                           *output
    ,unsigned long                   frameCount
    ,const PaStreamCallbackTimeInfo *timeInfo
    ,PaStreamCallbackFlags           statusFlags
    ,void                           *userData
    )
{
    float           *out;
    AudioFile *p_data = (AudioFile*)userData;

    sf_count_t       num_read;

    out = (float*)output;
    p_data = (AudioFile*)userData;

    /* clear output buffer */
    memset(out, 0, sizeof(float) * frameCount * p_data->info.channels);

    /* read directly into output buffer */
    num_read = sf_read_float(p_data->file, out, frameCount * p_data->info.channels);
    
    /*  If we couldn't read a full frameCount of samples we've reached EOF */
    if (num_read < frameCount)
    {
        return paComplete;
    }
    
    return paContinue;
}

// https://github.com/hosackm/wavplayer/blob/master/src/wavplay.c
void AudioEngine::playFile(){
    
    PaError err = Pa_Initialize();

    if (err != paNoError){
        // fprintf( stderr, "Couldn't init the portaudio library!\n" );
        throw std::runtime_error("Error initing the AudioEngine");
    }

    if ( activeFile == NULL ){
        err = Pa_Terminate();

        throw std::runtime_error("No data to play, shutting down.\n");
    }

    err = Pa_OpenDefaultStream(
        &stream,
        0,
        activeFile->info.channels,
        paFloat32,
        activeFile->info.samplerate,
        FRAMES_PER_BUFFER,
        _paStreamCallback,
        activeFile
    );

    if( err != paNoError )
    {
        throw std::runtime_error("Error opening Stream.\n");
    }

    err = Pa_StartStream(stream);

    if (err != paNoError){
        throw std::runtime_error("Error starting Stream\n");
    }

    /* Run until EOF is reached */
    while(Pa_IsStreamActive(stream))
    {
        Pa_Sleep(100);
    }

    /* Close the soundfile */
    sf_close(activeFile->file);

    /*  Shut down portaudio */
    err = Pa_CloseStream(stream);
    if(err != paNoError)
    {
        throw std::runtime_error("Error closing Stream.\n");
    }
    
    err = Pa_Terminate();
    if(err != paNoError)
    {
        throw std::runtime_error("Error terminating Portaudio.\n");
    }

}


