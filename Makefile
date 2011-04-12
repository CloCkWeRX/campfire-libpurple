LIBNAME=libcampfire
.PHONY: all

all: $(LIBNAME)
 
PURPLE_LIBS = $(shell pkg-config --libs purple)

CC:=gcc
LD:=$(CC)
CFLAGS=-DPURPLE_PLUGINS
CFLAGS+=$(shell pkg-config --cflags purple)
#CFLAGS+=$(shell pkg-config --cflags pidgin)
CFLAGS+=-Wall
LDFLAGS=$(CFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(LIBNAME): campfire_im.o
	$(LD) $(LDFLAGS) -shared $< $(PURPLE_LIBS) -o $@

.PHONY: clean

clean:
	-rm *.o
	-rm $(LIBNAME)
