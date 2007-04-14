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
command.h \
dns.h \
ethernet.h \
regexp.h \
save_load.h \
wifi.h 

libexalt_la_SOURCES = \
libexalt.c \
command.c \
dns.c \
ethernet.c \
regexp.c \
save_load.c \
wifi.c \
$(EXALTHEADERS)

installed_headersdir = $(prefix)/include/exalt
installed_headers_DATA = $(ENIGMAHEADERS)

libexalt_la_LIBADD  =

libexalt_la_DEPENDENCIES = 

libexalt_la_LDFLAGS = -version-info 1:0:0
