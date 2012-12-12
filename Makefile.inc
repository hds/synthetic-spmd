#
# Author: Hayden Stainsby <hds@caffeineconceps.com>
# Last serious edit: 2012-11-09
#


MPICC = mpiCC
MKDIR = mkdir -p

PRJDIR = $(dir $(lastword $(MAKEFILE_LIST)))
SRCDIR = $(PRJDIR)src
OBJDIR = $(PRJDIR)obj
BINDIR = $(PRJDIR)bin

