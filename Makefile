CC=gcc
CFLAGS=-I.
DEPS=
OBJ=server.o page_manager.o  # Add page_manager.o to the list of object files
USERID=123456789

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: server page_manager  # Add page_manager to the list of targets

server: server.o
	$(CC) -o $@ $^ $(CFLAGS)

page_manager: page_manager.o  # Add a rule to build page_manager
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o server page_manager *.tar.gz

dist: tarball
tarball: clean
	tar -cvzf /tmp/$(USERID).tar.gz --exclude=./.vagrant . && mv /tmp/$(USERID).tar.gz .

