CXXFLAGS= -Wall -g -std=c++11 
LDFLAGS= -Wall -g -pthread -std=c++11 

COMPILE=g++ $(CXXFLAGS) -c
LINK=g++ $(LDFLAGS) -o

SRCDIR= $(wildcard *.cpp) \
	$(wildcard Handler/*.cpp) \
	$(wildcard ../Minion/*.cpp) \
	$(wildcard ../Minion/Handler/*.cpp) \
	#$(wildcard ../Application/*.cpp) \

OBJ = $(SRCDIR:.cpp=.o) \
	$(wildcard ../Application/*.o) \
	#$(wildcard ../Minion/*.o) \
	#$(wildcard ../Minion/Handler/*.o) \
# OBJDIR = $(SRCDIR:.cpp=.o)


BINDIR=../../../bin
BIN=$(BINDIR)/Monitor


all: $(BIN) 

$(BIN): $(OBJ)
	$(LINK) $(BIN) $(OBJ)

$(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
	$(COMPILE) $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

