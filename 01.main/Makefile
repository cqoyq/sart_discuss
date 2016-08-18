INCLUDE = -I../
          

#$(info ${PATH})
#$(info $(ConfigName))

ifeq ($(ConfigName),Debug)
CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 $(INCLUDE)
else
CXXFLAGS =	-O2 -Wall -fmessage-length=0 $(INCLUDE)
endif

LIBS = -lpthread \
	   -ldl

LIBPATH = -L/opt/lib

OBJS =	main.o

		

TARGET = run

$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS) $(LIBPATH)

all: $(TARGET)

debug: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)



