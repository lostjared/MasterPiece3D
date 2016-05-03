bin_PROGRAMS=ump3d
ump3d_SOURCES=game.cpp main.cpp mp.cpp



AM_CPPFLAGS = -Wall -pedantic $(CFLAGPREFIX) $(LIBSDL_CFLAGS) $(all_includes) -I${prefix}/include/mx
AM_LDFLAGS  = $(all_librarys) $(LDPREFIX) -L${prefix}/lib -lmx $(LIBSDL_LIBS) -lpng  -lz
