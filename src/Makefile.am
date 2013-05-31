ACLOCAL_AMFLAGS=-I m4

AM_CPPFLAGS = $(all_includes)
AM_CXXFLAGS = $(all_includes)

bin_PROGRAMS = streamermlt

streamermlt_SOURCES = streamermlt.cpp io.c io.h

streamermlt_CPPFLAGS = -fpermissive ${LIBMLT_CFLAGS}	

streamermlt_LDFLAGS = 

streamermlt_LDADD = \
    -lpthread \
	${LIBMLT_LIBS}


