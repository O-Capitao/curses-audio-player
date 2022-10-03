#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <ncurses.h>
#include <boost/date_time/posix_time/posix_time.hpp>

bool QUIT = false;
int globalTimer = 0;
boost::posix_time::ptime start_time, now;
WINDOW *my_win;

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		
                    /* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/

	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void destroy_win(WINDOW *local_win)
{	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
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
        
        wclear( my_win );
        wprintw( my_win, boost::posix_time::to_simple_string( now ).c_str() );
        wrefresh(my_win);
    
    }

}

int main()
{
    
    start_time = boost::posix_time::microsec_clock::local_time();
    now = start_time;

    int startx, starty, width, height;
    height = 3;
	width = 10;
	starty = (LINES - height) / 2;	/* Calculating for a center placement */
	startx = (COLS - width) / 2;	/* of the window */



    initscr();
    noecho();

    my_win = create_newwin(height, width, starty, startx);
    // Enable F Keys
    // The external variable stdscr specifies the default window 
    // used by functions that do not specify a window using an argument of type WINDOW *.
    keypad(stdscr, true);

    // Find stuff about current terminal
    int windowW, windowH;
    getmaxyx(stdscr, windowH, windowW);
	
    boost::thread renderT{renderThread};
    boost::thread inputT{eventHandlerThread};
    
    renderT.join();
    inputT.join();

    endwin();			/* End curses mode		  */


	return 0;
}