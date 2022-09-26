# Our program machdump should be fully c99 compliant
PRG=machdump
CFLAGS+=-Iinclude -I libtermcolor/src -std=c99
WARNINGS=-Wall -Wextra -Wpedantic

# We want all C files in the src directory to be converted to object files
src=$(wildcard src/*.c)
obj=${src:.c=.o}

# These produce release and debug versions the machdump tool
release: main.c ${obj} libtermcolor/libtermcolor.a
	$(info ${obj})
	${CC} ${CFLAGS} ${WARNINGS} -O2 $^ -o ${PRG}
debug: main.c ${obj} libtermcolor/libtermcolor.a
	$(info ${obj})
	${CC} ${CFLAGS} ${WARNINGS} -g $^ -o ${PRG}

# We use phony to avoid writing the path in full
.PHONY: libtermcolor
libtermcolor: libtermcolor/libtermcolor.a

# We have a libtermcolor.a dependency
libtermcolor/libtermcolor.a:
	${MAKE} -C libtermcolor static

# This removes all unnecessary binaries
clean:
	rm -f main ${obj} libtermcolor/libtermcolor.a
