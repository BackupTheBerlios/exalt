MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# Gettext
datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

INCLUDES	= \
 -Wall -g

lib_LTLIBRARIES = libexalt.la

EXTRA_DIST = 

ENIGMAHEADERS = \
libexalt.h \
exalt_command.h \
exalt_dns.h \
exalt_ethernet.h \
exalt_regexp.h \
exalt_save_load.h \
exalt_wifi.h 

libexalt_la_SOURCES = \
libexalt.c \
exalt_command.c \
exalt_dns.c \
exalt_ethernet.c \
exalt_regexp.c \
exalt_save_load.c \
exalt_wifi.c \
$(EXALTHEADERS)

installed_headersdir = $(prefix)/include/exalt
installed_headers_DATA = $(ENIGMAHEADERS)

libexalt_la_LIBADD  =

libexalt_la_DEPENDENCIES = 

libexalt_la_LDFLAGS = -version-info 1:0:0
