CXXFLAGS= -Wall -g -std=c++11 
LDFLAGS= -Wall -g -pthread -std=c++11 

COMPILE=g++ $(CXXFLAGS) -c
LINK=g++ $(LDFLAGS) -o

SRCDIR= $(wildcard *.cpp) \

OBJ = $(SRCDIR:.cpp=.o)
# OBJDIR = $(SRCDIR:.cpp=.o)


BINDIR=../../../bin
BIN=$(BINDIR)/Auditor


all: $(BIN) 

$(BIN): $(OBJ)
	$(LINK) $(BIN) $(OBJ)

$(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
	$(COMPILE) $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

