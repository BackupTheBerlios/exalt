#!/bin/sh

rm -rf autom4te.cache
rm -f aclocal.m4 ltmain.sh

echo "Running autopoint..." ; autopoint -f || :
echo "Running aclocal..." ; aclocal $ACLOCAL_FLAGS || exit 1
echo "Running autoheader..." ; autoheader || exit 1
echo "Running autoconf..." ; autoconf || exit 1
echo "Running libtoolize..." ; (libtoolize --copy --automake || glibtoolize --automake) || exit 1
echo "Running automake..." ; automake --add-missing --copy --gnu || exit 1
echo "Generating gettext mixer.pot template"; \
xgettext \
--output mixer.pot \
--output-dir=po \
--language=C \
--add-location \
--keyword=D_ \
--sort-by-file \
--copyright-holder="TODO" \
--foreign-user \
`find . -name "*.[ch]" -print` || exit 1

if [ -z "$NOCONFIGURE" ]; then
	./configure "$@"
fi
