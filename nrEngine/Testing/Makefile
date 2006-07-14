#LIBS= 'pkg-config --libs nrEngine' -lGLFWBinding
#INCLUDE= 'pkg-config --cflags /diverses/stuff/work/nrEngine-newRelease/output/lib/pkgconfig/nrEngine.pc'
LIBS= -L/usr/local/lib/ -lnrEngine -lnrEngine_GLFWBinding


all:
	g++ -g -o test test.cpp $(INCLUDE) $(LIBS)
	
clean:
	rm -rf *~ 
