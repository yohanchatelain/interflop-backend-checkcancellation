ACLOCAL_AMFLAGS=-I m4
lib_LTLIBRARIES = libinterflop_checkcancellation.la

libinterflop_checkcancellation_la_SOURCES = \
    interflop_check_float_max.cxx \
    @INTERFLOP_STDLIB_PATH@/include/interflop-stdlib/iostream/logger.c 

libinterflop_checkcancellation_la_CFLAGS = \
    -DBACKEND_HEADER="interflop_checkcancellation" \
    -I@INTERFLOP_STDLIB_PATH@/include/ \
    -fno-stack-protector -fPIC
libinterflop_checkcancellation_la_CXXFLAGS = \
    -DBACKEND_HEADER="interflop_checkcancellation" \
    -I@INTERFLOP_STDLIB_PATH@/include/ \
    -fno-stack-protector -fPIC

if WALL_CFLAGS
libinterflop_checkcancellation_la_CFLAGS += -Wall -Wextra -Wno-varargs
endif
if LINK_INTERFLOP_STDLIB
libinterflop_checkcancellation_la_LIBADD = @INTERFLOP_STDLIB_PATH@/lib/libinterflop_stdlib.la
endif
libinterflop_checkcancellation_la_includedir = $(includedir)/
