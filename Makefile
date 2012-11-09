#
# Author: Hayden Stainsby <hds@caffeineconceps.com>
# Last serious edit: 2012-11-09
#



include Makefile.inc

.PHONY: all clean

all:
	@echo "Building Synthetic SPMD"
	$(MAKE) -C $(SRCDIR)

clean:
	@echo "Cleaning up"
	$(MAKE) -C $(SRCDIR) clean



