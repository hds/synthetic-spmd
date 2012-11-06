#
# Synthetic SMPD Makefile
#
# Author: Hayden Stainsby <hds@caffeineconceps.com>
# Last serious edit: 2012-11-06
#

CC = gcc
MPICC = mpicc
MKDIR = mkdir -p

SRCDIR = .
OBJDIR = ../obj
BINDIR = ../bin

INCLUDES = 
LIBS = 
CCFLAGS = -g -Wall

TARGETS = $(BINDIR)/synthetic-spmd
OBJS = $(OBJDIR)/synthetic-spmd.o $(OBJDIR)/ss-timing.o

all: $(BINDIR) $(TARGETS)

$(OBJDIR)/%.o: %.c $(OBJDIR)
	$(MPICC) $(CFLAGS) -c -o $@ $<

$(BINDIR)/synthetic-spmd: $(OBJS)
	$(MPICC) $(CFLAGS) $(LIBS) -o $@ $^

$(OBJDIR):
	$(MKDIR) $@

$(BINDIR):
	$(MKDIR) $@

clean:
	rm -f $(BINDIR)/* $(OBJDIR)/*
	rmdir $(BINDIR)
	rmdir $(OBJDIR)
