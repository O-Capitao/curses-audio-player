#include "audio.hpp"
#include <string>
#include  <filesystem>
#include <math.h>

using namespace CursesAudioPlayer;

extern bool QUIT;
extern SpectrumInfoFrame *activeFrame;

AudioEngine::AudioEngine(){
    _calculateWindowFunction();
}

AudioEngine::~AudioEngine(){
    _unloadFile();
}


void AudioEngine::loadFile( const std::string& path){

    if (_data != NULL){
        _unloadFile();
    }

    _data = new AudioData();
    _data->file = sf_open( path.c_str(), SFM_READ, &_data->info);
    _data->plan = fftw_plan_dft_r2c_1d( FRAMES_PER_BUFFER, _data->fft_in, _data->fft_out, FFTW_MEASURE );

}

void AudioEngine::_unloadFile(){

    if (_data == NULL) return;

    sf_close(_data->file);

    delete _data;

    _data = NULL;

}

const SoundFileInfo &AudioEngine::getSoundFileInfo(){
    if (_data != NULL){
        return _data->sfInfo;
    } 
    throw std::runtime_error("No active File, dumping...\n");
}




int AudioEngine::_paStreamCallback(
    const void                     *input
    ,void                           *output
    ,unsigned long                   frameCount
    ,const PaStreamCallbackTimeInfo *timeInfo
    ,PaStreamCallbackFlags           statusFlags
    ,void                           *userData
    )
{

    if (QUIT){
        return paComplete;
    }

    float *out;
    AudioData *p_data = (AudioData*)userData;

    sf_count_t num_read;

    out = (float*)output;
    p_data = (AudioData*)userData;

    /* clear output buffer */
    memset(out, 0, sizeof(float) * frameCount * p_data->info.channels);

    /* read directly into output buffer */
    num_read = sf_read_float(p_data->file, out, frameCount * p_data->info.channels);
    
    // do Fourier transform stuff
    // float *in = (float*)input;
    _copyArray( out, p_data->fft_in, FRAMES_PER_BUFFER );
    // fftw_execute(p_data->plan);

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

    if ( _data == NULL ){
        err = Pa_Terminate();

        throw std::runtime_error("No data to play, shutting down.\n");
    }

    err = Pa_OpenDefaultStream(
        &stream,
        0,
        _data->info.channels,
        paFloat32,
        _data->info.samplerate,
        FRAMES_PER_BUFFER,
        _paStreamCallback,
        _data
    );

    if( err != paNoError )
    {
        throw std::runtime_error("Error opening Stream.\n");
    }

    err = Pa_StartStream(stream);

    if (err != paNoError){
        throw std::runtime_error("Error starting Stream\n");
    }
    PLAYING = true;
    /* Run until EOF is reached */
    while(Pa_IsStreamActive(stream))
    {
        Pa_Sleep(100);
    }

    closeFile();
}

void AudioEngine::pauseFile(){
    if (PLAYING){
        PaError err = Pa_CloseStream( stream );

        if (err != paNoError){
            throw std::runtime_error("Error Stopping stream\n");
        }
    }
}

void AudioEngine::closeFile(){
    
    PaError err;

    PLAYING = false;
    /* Close the soundfile */
    sf_close(_data->file);

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

void AudioEngine::_calculateWindowFunction(){

    for (int i = 0; i < FRAMES_PER_BUFFER; i++){

        _windowFunctionPoints[i] = pow( sin( M_PI * i / (FRAMES_PER_BUFFER - 1) ), 2);

    }

}


void AudioEngine::_multiplyArrays(double *arr1, double*arr2, double *out, int l){
    for (int i = 0; i < l; i++ ){
        out[i] = arr1[i] * arr2[i];
    }
}

void AudioEngine::_copyArray(float *src, double*tgt, int l){
    for (int i = 0; i < l; i++){
        tgt[i] = src[i];
    }
}



