MAINTAINERCLEANFILES = Makefile.in

AUTOMAKE_OPTIONS = 1.4 foreign

# Gettext
datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

INCLUDES	= \
 -Wall -g @LIBEXALT_CFLAGS@

lib_LTLIBRARIES = libexalt.la

EXTRA_DIST = 

EXALTHEADERS = \
libexalt.h \
exalt_command.h \
exalt_dns.h \
exalt_ethernet.h \
exalt_regexp.h \
exalt_save_load.h \
exalt_wifi.h \
exalt_wifi_info.h

libexalt_la_SOURCES = \
libexalt.c \
exalt_command.c \
exalt_dns.c \
exalt_ethernet.c \
exalt_regexp.c \
exalt_save_load.c \
exalt_wifi.c \
exalt_wifi_info.c \
$(EXALTHEADERS)

installed_headersdir = $(prefix)/include/exalt
installed_headers_DATA = $(EXALTHEADERS)

libexalt_la_LIBADD  = @LIBEXALT_LIBS@

libexalt_la_DEPENDENCIES = 

libexalt_la_LDFLAGS = -version-info 1:0:0
