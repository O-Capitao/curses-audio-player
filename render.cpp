#include <render.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "audio.hpp"

using namespace CursesAudioPlayer;

extern bool QUIT;
// extern SpectrumInfoFrame *activeFrame;

RenderWorker::RenderWorker(){
    
    _start_time = boost::posix_time::microsec_clock::local_time();
    _now  = _start_time;

    _topTerminalSize = {120, 24};
    _cmdWindow_size = {120,2};
    _spectrumWindow_size = { 120, 9 };
    _playbackInfoWindow_size = {60, (_topTerminalSize.y - _cmdWindow_size.y - _spectrumWindow_size.y )};
    _fileInfoWindow_size = { 60, (_topTerminalSize.y - _cmdWindow_size.y - _spectrumWindow_size.y )};

    _cmdWindow = newwin(
        _cmdWindow_size.y,
        _cmdWindow_size.x,
        _topTerminalSize.y - _cmdWindow_size.y, 
        0
    );

    _spectrumWindow = newwin(
        _spectrumWindow_size.y,
        _spectrumWindow_size.x,
        0,
        0
    );

    _playbackInfoWindow = newwin(
        _playbackInfoWindow_size.y,
        _playbackInfoWindow_size.x,
        _spectrumWindow_size.y,
        0
    );

    _fileInfoWindow = newwin(
        _playbackInfoWindow_size.y,
        _playbackInfoWindow_size.x,
        _spectrumWindow_size.y,
        60
    );

}

RenderWorker::~RenderWorker(){
    
}


void RenderWorker::run(){

    while (!QUIT){
    
        _now = boost::posix_time::microsec_clock::local_time();

        ExternalAudioData engineData = _engine->getAudioData();

        wclear( _cmdWindow );
        wclear( _spectrumWindow );
        wclear( _playbackInfoWindow );
        wclear( _fileInfoWindow );

        std::string commands = "[P]lay/[P]ause   [L]oad File   [Q]uit";

        wattron(_cmdWindow, A_BOLD );
        mvwprintw( _cmdWindow, 1, 1, boost::posix_time::to_simple_string( _now ).c_str() );

        wattroff(_cmdWindow, A_BOLD );
        mvwprintw( _cmdWindow, 1, _topTerminalSize.x - commands.size() - 2, commands.c_str() );

        std::string dataStringified = engineData.stringify();

        mvwprintw( _fileInfoWindow, 1,1, dataStringified.c_str());

        wborder(_playbackInfoWindow, '|', '|', '-', '-', '+', '+', '+', '+');
        wborder(_fileInfoWindow, '|', '|', '-', '-', '+', '+', '+', '+');

        wrefresh( _cmdWindow );
        wrefresh( _spectrumWindow );
        wrefresh( _playbackInfoWindow );
        wrefresh( _fileInfoWindow );

        boost::this_thread::sleep(boost::posix_time::millisec(83));   
    }
}