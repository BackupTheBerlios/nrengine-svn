#LIBS= 'pkg-config --libs /diverses/stuff/work/nrEngine-newRelease/output/lib/pkgconfig/nrEngine.pc'
#INCLUDE= 'pkg-config --cflags /diverses/stuff/work/nrEngine-newRelease/output/lib/pkgconfig/nrEngine.pc'
LIBS= -L/usr/local/lib/ -lnrEngine


all:
	g++ -g -o test test.cpp $(INCLUDE) $(LIBS)
	
clean:
	rm -rf *~ 
