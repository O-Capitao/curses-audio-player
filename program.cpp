#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <ncurses.h>
#include <boost/date_time/posix_time/posix_time.hpp>

bool QUIT = false;
int globalTimer = 0;
boost::posix_time::ptime start_time, now;


void wait(int seconds){
  boost::this_thread::sleep_for(boost::chrono::seconds{seconds});
}

void eventHandlerThread(){
  char c;
  while (!QUIT){
    c = getch();
    if (c == 'q'){
        QUIT = true;
    }
  }
}

void renderThread(){
    while (!QUIT){
        now = boost::posix_time::microsec_clock::local_time();
        clear();
        printw( boost::posix_time::to_simple_string( now ).c_str() );
        refresh();
    }
}

int main()
{
    // boost::thread t{thread};
    start_time = boost::posix_time::microsec_clock::local_time();
    now = start_time;

    initscr();			/* Start curses mode 		  */
    renderThread();
	/* Wait for user input */
	endwin();			/* End curses mode		  */


	return 0;
}