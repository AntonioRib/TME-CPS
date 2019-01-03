SUBDIRS = $(wildcard src/Servers/AuditingHub/.) \
	$(wildcard src/Servers/Minion/.) \
	$(wildcard src/Servers/Monitor/.) \
	$(wildcard src/Servers/Application/.) \
	$(wildcard src/Clients/Auditor/.) \
	$(wildcard src/Clients/Developer/.) \
	$(wildcard src/Clients/SysAdmin/.) 
	 #$(wildcard src/Utilities/.)

RMSUBDIRS = $(wildcard src/Servers/AuditingHub/.) \
	$(wildcard src/Servers/Minion/.) \
	$(wildcard src/Servers/Monitor/.) \
		$(wildcard src/Servers/Application/.) \
	$(wildcard src/Clients/Auditor/.) \
	$(wildcard src/Clients/Developer/.) \
	$(wildcard src/Clients/SysAdmin/.) 
	#$(wildcard src/Utilities/.)

.PHONY : all $(SUBDIRS)
all : $(SUBDIRS)
$(SUBDIRS) :
	$(MAKE) -C $@ clean all

# clean : $(RMSUBDIRS)
# $(RMSUBDIRS) :
# 	$(MAKE) -C $@ clean

# OBJ = $(SRCDIR:.cpp=.o)
# # OBJDIR = $(SRCDIR:.cpp=.o)


# BINDIR=Debug
# BIN=$(BINDIR)/Monitor


# all: $(BIN) 

# $(BIN): $(OBJ)
# 	$(LINK) $(BIN) $(OBJ)

# $(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
# 	$(COMPILE) $< -o $@

# clean:
# 	rm -f $(OBJ) $(BIN)

