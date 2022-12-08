lib_LTLIBRARIES = libinterflop_checkcancellation.la

libinterflop_checkcancellation_la_SOURCES = \
    interflop_checkcancellation.cxx \
    @INTERFLOP_STDLIB_PATH@/include/interflop-stdlib/iostream/logger.c 

libinterflop_checkcancellation_la_CFLAGS = \
    -DBACKEND_HEADER="interflop_checkcancellation" \
    -I@INTERFLOP_STDLIB_PATH@/include/ \
    -fno-stack-protector
libinterflop_checkcancellation_la_CXXFLAGS = \
    -DBACKEND_HEADER="interflop_checkcancellation" \
    -I@INTERFLOP_STDLIB_PATH@/include/ \
    -fno-stack-protector

if WALL_CFLAGS
libinterflop_checkcancellation_la_CFLAGS += -Wall -Wextra -Wno-varargs -Og -g
endif
if LINK_INTERFLOP_STDLIB
libinterflop_checkcancellation_la_LIBADD = @INTERFLOP_STDLIB_PATH@/lib/libinterflop_stdlib.la
endif
libinterflop_checkcancellation_la_includedir = $(includedir)/
include_HEADERS = interflop_checkcancellation.h