#pragma once

#include <ncurses.h>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace CursesAudioPlayer{

    struct Int2DArray{
        int x, y;

        Int2DArray(){
            x = 0;
            y = 0;
        }

        Int2DArray(int _x, int _y){
            x = _x;
            y = _y;
        }
    };

    class RenderWorker {

        WINDOW *_cmdWindow;
        WINDOW *_spectrumWindow;
        WINDOW *_playbackInfoWindow;
        WINDOW *_fileInfoWindow;
        
        Int2DArray _topTerminalSize,
            _cmdWindow_size,
            _spectrumWindow_size,
            _playbackInfoWindow_size,
            _fileInfoWindow_size;

        boost::posix_time::ptime _start_time, _now;

        public:
            
            RenderWorker();
            ~RenderWorker();
            void run();


    };

};

