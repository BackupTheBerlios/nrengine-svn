
pkgincludedir = $(includedir)/@PACKAGE@/Bindings/glfw
pkginclude_HEADERS = Binding.h Event.h

pkglibdir = $(libdir)
 
CC=g++

INCLUDES = -I$(top_srcdir)/Bindings/glfw/ -I$(top_srcdir)/nrEngine/include
CFLAGS= -O2 -Wall -Werror

OBJECTS= Binding.o Task.o
LIBS= -lglfw -lGL -lGLU -lX11 -pthread
LDFLAGS= -L/lib -L/usr/lib -L/usr/X11R6/lib -Llib

NAME= libnrEngine_GLFWBinding.so

	
all: $(NAME)

$(OBJECTS): %.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

libnrEngine_GLFWBinding.so: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LDFLAGS) $(LIBS) $(PLUGIN_FLAGS)

	
install-exec-am:
install:
	cp $(NAME) $(DESTDIR)$(pkglibdir)/
	mkdir -p $(DESTDIR)$(includedir)/@PACKAGE@/Bindings
	mkdir -p $(DESTDIR)$(pkgincludedir)
	cp Binding.h $(DESTDIR)$(pkgincludedir)
	cp Event.h $(DESTDIR)$(pkgincludedir)
	
uninstall-am:
uninstall:
	rm -f $(DESTDIR)$(pkglibdir)/$(NAME)
	
clean: 
	rm -f core.* core $(OBJECTS)
	rm -f *~
	rm -f DEADJOE
	rm -f *.bb *.bbg *.da *.gcov *.out
	rm -f $(NAME)
