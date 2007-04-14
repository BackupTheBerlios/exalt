## Process this file with automake to produce Makefile.in

SUBDIRS = src

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess \
		config.h.in config.sub configure install-sh \
		ltconfig ltmain.sh missing mkinstalldirs \
		stamp-h.in build-stamp configure-stamp depcomp \
		etk_docs.tar.gz \
		etk.pc \
		debian/changelog


EXTRA_DIST = \
AUTHORS \
COPYING \
README \
ABOUT-NLS \
debian/changelog \
debian/changelog.in \
debian/control \
debian/copyright \
debian/rules 

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = exalt.pc
ACLOCAL_AMFLAGS = -I m4
