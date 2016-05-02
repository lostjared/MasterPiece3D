#!/bin/sh
echo "setting up enviorment"
aclocal >/dev/null 2>&1
autoheader
libtoolize --force --copy 
automake  --foreign --add-missing --copy 
autoconf >/dev/null 
echo "complete "
