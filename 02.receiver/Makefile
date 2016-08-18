INCLUDE = -I/opt/src/bingo/libbingo \
		  -I/opt/software/boost_1_59_0
          

#$(info ${PATH})
#$(info $(ConfigName))

ifeq ($(ConfigName),Debug)
CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 $(INCLUDE)
else
CXXFLAGS =	-O2 -Wall -fmessage-length=0 $(INCLUDE)
endif

LIBS = -lpthread \
	   -ldl \
	   -lbingo \
	   -lboost_filesystem \
	   -lboost_random \
	   -lboost_regex \
       -lboost_chrono \
	   -lboost_timer \
	   -lboost_system \
	   -lboost_thread \
	   -lboost_context \
	   -lboost_coroutine

LIBPATH = -L/opt/lib \
		  -L/opt/software/boost_1_59_0/stage/lib

OBJS =	main.o

		

TARGET = receiver

$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS) $(LIBPATH)

all: $(TARGET)

debug: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)



