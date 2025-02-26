#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <ncurses.h>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sys/ioctl.h>

#include "render.hpp"
#include "audio.hpp"

using namespace CursesAudioPlayer;

// Globl Quit signal
bool QUIT = false;
bool PLAY = false; // if false its stopped.
bool USE_TUI = true;

void eventHandlerThread(){

  char c;
  while (!QUIT){

    c = getch();
  
    if (c == 'q' || c == 'Q'){
        QUIT = true;
    } else if ( c == 'p' ){
        PLAY = !PLAY;
    }
    
    boost::this_thread::sleep(boost::posix_time::millisec(100));   
  }
}



/****
 *  MAIN
 */
int main(int argc, char *argv[])
{
    sleep(4);
    if (argc < 2){
        throw std::runtime_error("Please supply a path to the audio file");
        return 1;
    }

    printf("starting....\n");

    std::string file_path = argv[1];

    
    AudioEngine engine;
    engine.loadFile(file_path.c_str());
    
    // Learn current size of terminal,
    // after exit, we need to put it back
    winsize w;
    ioctl(0, TIOCGWINSZ, &w);



    
    
    boost::thread inputT{eventHandlerThread};
    boost::thread playT{boost::bind(&AudioEngine::playFile, &engine)};

    // optional curses TUI thread
    if (USE_TUI){

        // Set Terminal Size to 24 rows, 120 cols
        printf("\e[8;24;120t");

        // start the NCurses env.
        initscr();
        noecho();
        curs_set(0);
        keypad(stdscr, true);

        // init objects
        RenderWorker renderWorker;
        renderWorker.registerAudioEngine(&engine);

        boost::thread renderT = boost::thread{boost::bind(&RenderWorker::run, &renderWorker)};
        renderT.join();
        
        // End curses mode		  
        endwin();	
        // restore initial terminal size
        printf("\e[8;%i;%it",w.ws_row, w.ws_col);

    }

    inputT.join();
    playT.join();

    
		

	return 0;
}