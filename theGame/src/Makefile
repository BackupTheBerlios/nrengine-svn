PREFIX=

CC=g++
LIBS=  -L/usr/lib -L/usr/X11R6/lib -L/HPS/usr7/tevs/local/lib\
	   -lnrEngine -lnrEngine_GLFWBinding

INCLUDES= -I/usr/local/include/ -I/HPS/usr7/tevs/local/include
OPT_FLAGS= -Wall -Werror -g -march=i686

CFLAGS=$(OPT_FLAGS)

LDFLAGS= 

DESTDIR=
OBJECTS= Main.o App.o GameTask.o

SIM_BIN=theGame
BINS=$(SIM_BIN)

all: app

$(OBJECTS): %.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

app: $(OBJECTS)
	$(CC) $(CFLAGS) -o ../$(SIM_BIN) $(OBJECTS) $(LIBS) $(LDFLAGS)

clean: 
	rm -f core.* core $(OBJECTS)
	rm -f *~
	rm -f DEADJOE
	rm -f *.bb *.bbg *.da *.gcov *.out

uninstall:
	rm -f $(PREFIX)/$(BINS)

