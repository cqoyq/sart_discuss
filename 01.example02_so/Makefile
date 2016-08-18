CPPS = plugin.cpp \
	   my.cpp \
	   ../01.example_so/atest.cpp
	   
LIBS = -lpthread
	   
LIBPATH = -L/opt/lib

INCLUDE = 

TARGET = libexample02.so

all:
	$(CXX)  -fPIC  -shared -o $(TARGET) $(CPPS) $(INCLUDE) $(LIBS) $(LIBPATH);cp $(TARGET) ../../../../lib

debug:
	$(CXX)  -g -fPIC  -shared -o $(TARGET) $(CPPS) $(INCLUDE) $(LIBS) $(LIBPATH);cp $(TARGET) ../../../../lib

clean:
	rm -f $(TARGET)