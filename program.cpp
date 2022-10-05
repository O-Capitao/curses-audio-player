#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <ncurses.h>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "render.hpp"

using namespace CursesAudioPlayer;

// Globl Quit signal
bool QUIT = false;

void eventHandlerThread(){

  char c;
  while (!QUIT){

    c = getch();
  
    if (c == 'q' || c == 'Q'){
        QUIT = true;
    }
  
  }

}



/****
 *  MAIN
 */
int main()
{
    // interval for me to attach the debugger
    // sleep(5);

    // Set Terminal Size
    printf("\e[8;24;120t");
    // Update NCurses drawable Area Accordingly
    // resizeterm(24, 120);

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
    
    // Cleanup.
    // join main threads
    renderT.join();
    inputT.join();

    // End curses mode		  
    endwin();			

	return 0;
}