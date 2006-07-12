CC=g++

INCLUDES = -I$(top_srcdir)/Plugins/rdtscTimeSource/ -I$(top_srcdir)/nrEngine/include
CFLAGS= -O2 -Wall -Werror

OBJECTS= Plugin.o Cpu.o TimeSource.o


NAME= rdtscTimeSource.so


	
all: $(NAME)

$(OBJECTS): %.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

rdtscTimeSource.so: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LIBS) $(LDFLAGS) $(PLUGIN_FLAGS)

	
install-exec-am:
install:
	cp $(NAME) $(DESTDIR)$(pkglibdir)

uninstall-am:
uninstall:
	rm -f $(DESTDIR)$(pkglibdir)/$(NAME)
	
clean: 
	rm -f core.* core $(OBJECTS)
	rm -f *~
	rm -f DEADJOE
	rm -f *.bb *.bbg *.da *.gcov *.out
	rm -f $(NAME)
