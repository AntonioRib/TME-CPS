CXXFLAGS= -Wall -g -std=c++11 -I../../../lib/tpm2
LDFLAGS= -Wall -g -pthread -std=c++11

COMPILE=g++ $(CXXFLAGS) -c
LINK=g++ $(LDFLAGS) -o

SRCDIR= $(wildcard *.cpp) \

OBJ = $(SRCDIR:.cpp=.o)
# OBJDIR = $(SRCDIR:.cpp=.o)


BINDIR=../../../bin
BIN=$(BINDIR)/Developer


all: $(BIN) 

$(BIN): $(OBJ)
	$(LINK) $(BIN) $(OBJ) -ltss2-mu  -ltss2-sys -ltss2-esys -ltss2-tcti-device -ltss2-tcti-mssim -L/usr/local/lib/ 

$(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
	$(COMPILE) $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

