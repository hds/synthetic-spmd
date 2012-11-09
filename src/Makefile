#
# Synthetic SMPD Makefile
#
# Author: Hayden Stainsby <hds@caffeineconceps.com>
# Last serious edit: 2012-11-09
#

include ../Makefile.inc

INCLUDES = 
LIBS = 
CCFLAGS = -g -Wall

TARGETS = $(BINDIR)/synthetic-spmd
OBJS = $(OBJDIR)/synthetic-spmd.o $(OBJDIR)/ss-timing.o $(OBJDIR)/ss-work.o

.PHONY: all clean

all: $(BINDIR) $(TARGETS)

$(OBJDIR)/%.o: %.c $(OBJDIR)
	$(MPICC) $(CCFLAGS) -c -o $@ $<

$(BINDIR)/synthetic-spmd: $(OBJS)
	$(MPICC) $(CCFLAGS) $(LIBS) -o $@ $^

$(OBJDIR):
	$(MKDIR) $@

$(BINDIR):
	$(MKDIR) $@

clean:
	rm -f $(BINDIR)/* $(OBJDIR)/*
	rmdir $(BINDIR)
	rmdir $(OBJDIR)
