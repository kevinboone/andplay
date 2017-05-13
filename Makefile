PREFIX=/usr
MYCFLAGS=-Wall -fpie -fpic -pie $(CFLAGS)
LIBS=-lOpenSLES
MYLDFLAGS= $(LDFLAGS)
VERSION=0.0.1

BINARY=andplay

all: $(BINARY)

main.o: main.c
	$(CC) $(CFLAGS) -s -o main.o -c -DVERSION=\"$(VERSION)\" $(INCLUDES) $(LIBS) main.c $(EXTRA_OBJS)

$(BINARY): main.o
	$(CC) $(LDFLAGS) -s -o andplay main.o $(LIBS) 

install: all
	mkdir -p $(DESTDIR)/$(PREFIX)/bin/
	cp -p $(BINARY) $(DESTDIR)/$(PREFIX)/bin/

clean:
	rm -rf $(BINARY) *.o 
