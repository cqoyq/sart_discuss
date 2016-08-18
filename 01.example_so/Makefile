CPPS = plugin.cpp \
	   user.cpp \
	   atest.cpp
	   
LIBS = -lpthread
	   
LIBPATH = -L/opt/lib

INCLUDE = 

TARGET = libexample.so

all:
	$(CXX)  -fPIC  -shared -o $(TARGET) $(CPPS) $(INCLUDE) $(LIBS) $(LIBPATH);cp $(TARGET) ../../../../lib

debug:
	$(CXX)  -g -fPIC  -shared -o $(TARGET) $(CPPS) $(INCLUDE) $(LIBS) $(LIBPATH);cp $(TARGET) ../../../../lib

clean:
	rm -f $(TARGET)