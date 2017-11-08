#
# Makefile - build cross llvm for linux / cortex-m
# Copyright (C) 2017 Heiko Abraham
# All rights reserved.
#

.PHONY: all
all:
	( cd makefiles && make -f Makefile.dl )
	( cd makefiles && make -f Makefile.binutils )
	( cd makefiles && make -f Makefile.cmake )
	( cd makefiles && make -f Makefile.llvm )


.PHONY: install
install:
	( cd makefiles && make -f Makefile.llvm install )


.PHONY: uninstall
uninstall:
	( cd makefiles && make -f Makefile.llvm uninstall )


.PHONY: clean
clean:
	( cd makefiles && make -f Makefile.binutils clean )
	( cd makefiles && make -f Makefile.cmake    clean )
	( cd makefiles && make -f Makefile.llvm     clean )
	( cd makefiles && make -f Makefile.llvm     clean )
	( cd makefiles && make -f Makefile.def      clean_build_dir )
	

.PHONY: download
download:
	( cd makefiles && make -f Makefile.dl )


.PHONY: help
help:
	@echo "Makefile to build llvm as cross compiler for ARM Cortex-M"
	@echo " "
	@echo "make [INST_PREFIX=/path]             - build cross compiler"
	@echo "make [INST_PREFIX=/path] (un)install - (un)install cross compiler"
	@echo "make clean                           - clean object files"
	@echo "make download                        - download llvm/binutils source if required"
