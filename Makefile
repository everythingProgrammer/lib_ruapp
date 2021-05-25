GCC=gcc
CFLAGS=-c -Wall -Werror -fpic
INCLUDE=ru_app_lib/
BINDIR=build/
OBJDIR=build/
LIBDIR=build/
SRCDIR=ru_app_lib/
SRCEXT=c
OBJEXT=o
LFLAG=-lsysrepo
SOURCES_LIB=$(wildcard $(SRCDIR)*.c)
OBJECTS_LIB := $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(SOURCES_LIB:.$(SRCEXT)=.$(OBJEXT)))
# $@, The name of target
# $^, The name of all depencency 
# $<, The name of first depencency
# $?, The name of all depencencies newer than the target space seperated file
# $+, The name of all dependencies seperated by space with dupicated names included and in the same order as in the rules.

all: $(BINDIR)libruapp.so $(BINDIR)ruapp
$(BINDIR)libruapp.so: $(OBJECTS_LIB)
	@echo "Lnking all library object files"
	$(GCC) $^ -shared -o $@ $(LFLAG)
	@echo "Done, created $@"    

$(OBJDIR)%.o: $(SRCDIR)%.c
	@echo "Compiling $<"
	$(GCC) $(CFLAGS) -I$(INCLUDE)  $<  -o $@
	@echo "Done"

$(BINDIR)ruapp:
	@echo "Creating ruapp"
	@echo Link PATH: $(LD_LIBRARY_PATH)
	$(GCC) -L$(LIBDIR) main.c -I$(INCLUDE) -o $@ -lruapp

clean:
	rm -rf $(BINDIR)*.o $(BINDIR)*.so $(BINDIR)ruapp


