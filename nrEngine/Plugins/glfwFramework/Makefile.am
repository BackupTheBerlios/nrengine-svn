CC=g++

INCLUDES = -I$(top_srcdir)/Plugins/glfwFramework/ -I$(top_srcdir)/nrEngine/include
CFLAGS= -O2 -Wall -Werror

OBJECTS= glfwTask.o Plugin.o
LIBS= -lglfw -lGL -lGLU -lX11 -lpthread -lm
LDFLAGS= -L/lib -L/usr/lib -Llib

NAME= glfwBindings.so


	
all: $(NAME)

$(OBJECTS): %.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

glfwBindings.so: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LDFLAGS) $(LIBS) $(PLUGIN_FLAGS)

	
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
