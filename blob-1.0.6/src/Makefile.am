# -*- makefile -*-
###########################################################################
## Filename:      Makefile.am
## Version:       $Id: Makefile.am,v 1.11 2000/01/20 22:46:22 erikm Exp $
## Copyright:     Copyright (C) 1999, Erik Mouw
## Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
## Description:   Makefile
## Created at:    Tue Aug 17 17:25:56 1999
## Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
## Modified at:   Tue Sep 28 23:33:26 1999
###########################################################################


bin_PROGRAMS	= \
	blob-elf32 \
	blob


# WARNING: start.S *must* be the first file, otherwise the target will
# be linked in the wrong order!
blob_elf32_SOURCES	= \
	start.S \
	clock.c \
	command.c \
	main.c \
	flash.c \
	serial.c \
	time.c \
	util.c \
	uucodec.c


blob_SOURCES		=


EXTRA_DIST		= \
	ld-script


blob_elf32_LDFLAGS	+= \
	-Wl,-T,ld-script


blob_elf32_LDADD	+= \
	-lgcc


INCLUDES		+= \
	-I${top_builddir}/include \
	-I${top_srcdir}/include


blob: blob-elf32
	$(OBJCOPY) $(OCFLAGS) $< $@


CLEANFILES	= *~


DISTCLEANFILES	= .deps/*.P
