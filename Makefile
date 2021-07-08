########################################################################
####################### Makefile Template ##############################
########################################################################

# Compiler settings - Can be customized.
CC = gcc
CXXFLAGS = -std=c11 -Wall
LDFLAGS = 

# Makefile settings - Can be customized.
APPNAME = brace
EXT = .c
SRCDIR = src
HEADERDIR = $(SRCDIR)/headers
BINDIR = bin
OBJDIR = $(BINDIR)/obj
DEPDIR = $(BINDIR)/dep

############## Do not change anything from here downwards! #############
SRC = $(wildcard $(SRCDIR)/*$(EXT))
# HEADERS = $(wildcard $(HEADERDIR)/*.h)
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)
DEPS = $(SRC:$(SRCDIR)/%$(EXT)=$(DEPDIR)/%.d)
APP = $(BINDIR)/$(APPNAME)
DEP = $(OBJ:$(OBJDIR)/%.o=%.d)
# UNIX-based OS variables & settings
RM = rm
MKDIR = mkdir
DELOBJ = $(OBJ)
# Windows OS variables & settings
DEL = del
EXE = .exe
WDELOBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)\\%.o)

########################################################################
####################### Targets beginning here #########################
########################################################################

.MAIN: $(APP)
all: $(APP)

# Builds the app
$(APP): $(OBJ) | makedirs
	@# printf "compiling $(notdir $<) into $(notdir $@)..."
	@printf "compiling final product $(notdir $@)..."
	@$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@printf "\b\b done!\n"

# Creates the dependecy rules
%.d: $(SRCDIR)/%$(EXT) | makedirs
# $(DEPS): $(SRCDIR)/%$(EXT) | makedirs
	@printf "compiling $(notdir $<) into $(notdir $@)..."
	@# $(CPP) $(CFLAGS) $< -MM -MT $(@:%.d=$(OBJDIR)/%.o) >$@
	@$(CPP) $(CFLAGS) $< -MM -MT $(DEPS) >$@
	@printf "\b\b done!\n"

# Includes all .h files
-include $(DEPS)

# Building rule for .o files and its .c/.cpp in combination with all .h
# $(OBJDIR)/%.o: $(SRCDIR)/%$(EXT) | makedirs
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT) | makedirs
	@printf "compiling $(notdir $<) into $(notdir $@)..."
	@$(CC) $(CXXFLAGS) -I $(HEADERDIR) -o $@ -c $<
	@printf "\b\b done!\n"

################### Cleaning rules for Unix-based OS ###################
# Cleans complete project
.PHONY: clean
clean:
	@# $(RM) $(DELOBJ) $(DEP) $(APP)
	@$(RM) -r $(OBJDIR)
	@$(RM) -r $(DEPDIR)
	@$(RM) -r $(BINDIR)

# # Cleans only all files with the extension .d
# .PHONY: cleandep
# cleandep:
# 	$(RM) $(DEP)

# #################### Cleaning rules for Windows OS #####################
# # Cleans complete project
# .PHONY: cleanw
# cleanw:
# 	$(DEL) $(WDELOBJ) $(DEP) $(APPNAME)$(EXE)

# # Cleans only all files with the extension .d
# .PHONY: cleandepw
# cleandepw:
# 	$(DEL) $(DEP)

.PHONY: run
run: $(APP)
	@printf "============ Running \"$(APP)\" ============\n\n"
	@$(APP)

.PHONY: run-debug
run-debug: $(APP)
	@printf "============ Running \"$(APP)\" in debug mode ============\n\n"
	@$(APP) --debug

routine: $(APP) run clean

.PHONY: makedirs
makedirs:
	@$(MKDIR) -p $(BINDIR)
	@$(MKDIR) -p $(OBJDIR)
	@$(MKDIR) -p $(DEPDIR)

remake: clean $(APP)
