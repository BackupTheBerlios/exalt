SUBDIRS = po data

MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS = -v \
-id $(top_srcdir) \
-fd $(top_srcdir)

files_DATA = \
module.desktop \
e-module-exalt.edj \
exalt.edj 

EXTRA_DIST = \
$(files_DATA) \
exalt.edc \
exalt_icon.png \
vol.png \
e_modules-exalt.spec.in \
e_modules-exalt.spec

# data files for the module
filesdir = $(datadir)

# the module .so file
INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @EXALT_CFLAGS@ \
	 @e_cflags@

pkgdir                 = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES        = module.la
module_la_SOURCES      = e_mod_main.c \
		       e_mod_main.h \
		       e_mod_types.h \
		       e_mod_config.c

module_la_LIBADD       = @e_libs@ @EXALT_LIBS@
module_la_LDFLAGS      = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h


exalt.edj:  exalt.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf exalt.edj *~
uninstall:
	rm -rf $(datadir)
