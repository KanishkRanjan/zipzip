flags=-O2 -Wall -std=c2x
ldflags=

.PHONY: all clean debug

all: clean zipzip

zipzip: zipzip.o
	cc $(flags) $^ -o $@ $(ldflags)

zipzip.o: zipzip.c zipzip.h
	cc $(flags) -c $<

clean:
	rm -rf *.o zipzip zipzip_debug zipzip_debug.dSYM

# AddressSanitizer build: on a crash it prints the exact line and why
debug: zipzip.c zipzip.h
	cc -g -O0 -fsanitize=address,undefined -Wall -std=c2x zipzip.c -o zipzip_debug $(ldflags)
