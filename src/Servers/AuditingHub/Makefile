CXXFLAGS= -Wall -g -std=c++11 
LDFLAGS= -Wall -g -pthread -std=c++11

COMPILE=g++ $(CXXFLAGS) -c
LINK=g++ $(LDFLAGS) -o

SRCDIR= $(wildcard *.cpp) \
	$(wildcard Handler/*.cpp) \

OBJ = $(SRCDIR:.cpp=.o)
# OBJDIR = $(SRCDIR:.cpp=.o)


# BINDIR=../../../bin
BINDIR=.
BIN=$(BINDIR)/AuditingHub


all: $(BIN) 

$(BIN): $(OBJ)
	$(LINK) $(BIN) $(OBJ) -ltss2-mu  -ltss2-sys -ltss2-esys -ltss2-tcti-device -ltss2-tcti-mssim -L/usr/local/lib/

$(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
	$(COMPILE) $< -o $@ 
clean:
	rm -f $(OBJ) $(BIN)

