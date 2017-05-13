PREFIX=/usr
MYCFLAGS=-Wall -fpie -fpic $(CFLAGS)
LIBS=-lOpenSLES
MYLDFLAGS= -pie $(LDFLAGS)
VERSION=0.0.1

BINARY=andplay

all: $(BINARY)

main.o: main.c
	$(CC) $(MYCFLAGS) -s -o main.o -c -DVERSION=\"$(VERSION)\" $(INCLUDES) $(LIBS) main.c $(EXTRA_OBJS)

$(BINARY): main.o
	$(CC) $(MYLDFLAGS) -s -o andplay main.o $(LIBS) 

install: all
	mkdir -p $(DESTDIR)/$(PREFIX)/bin/
	cp -p $(BINARY) $(DESTDIR)/$(PREFIX)/bin/

clean:
	rm -rf $(BINARY) *.o 
