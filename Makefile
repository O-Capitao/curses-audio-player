CFLAGS = -std=c++17 -I. -I/opt/homebrew/Cellar/boost/1.80.0/include/boost -I/opt/homebrew/include

LDFLAGS = -L/opt/homebrew/lib -l boost_thread-mt -lboost_system -lboost_chrono -lncurses

SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)
OBJS = $(wildcard *.o) $(wildcard */*.o)

OUTPUTFILE=player.a

player: program.cpp
	g++ $(CFLAGS) -o $(OUTPUTFILE) -g $(SOURCES) $(LDFLAGS) -Wall

clean:
	rm -f $(OUTPUTFILE) $(OBJS)