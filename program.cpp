#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <ncurses.h>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "render.hpp"
#include "audio.hpp"

using namespace CursesAudioPlayer;

// Globl Quit signal
bool QUIT = false;
bool PLAY = false; // if false its stopped.

SpectrumInfoFrame *activeFrame = NULL;

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
    // sleep(5);
    if (argc != 2){
        throw std::runtime_error("Please supply a path to the audio file");
        return 1;
    }

    printf("starting....\n");

    std::string file_path = argv[1];

    
    AudioEngine engine;
    engine.loadFile(file_path.c_str());

    // Set Terminal Size
    printf("\e[8;24;120t");

    // start the NCurses env.
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, true);

    // init objects
    RenderWorker renderWorker;
    
    // launch the separateThreads
    boost::thread renderT{boost::bind(&RenderWorker::run, &renderWorker)};
    boost::thread inputT{eventHandlerThread};
    boost::thread playT{boost::bind(&AudioEngine::playFile, &engine)};

    // Cleanup.
    // join main threads
    renderT.join();
    inputT.join();
    playT.join();

    
    // End curses mode		  
    endwin();			

	return 0;
}