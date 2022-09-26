CFLAGS   += -Isrc -fPIC -std=c99 -pedantic
WARNINGS += -Wall -Wextra
SRC      := $(wildcard src/*.c)
OBJ      := ${SRC:.c=.o}
PRG      := libtermcolor

ifeq ($(shell uname), Darwin)
AR = /usr/bin/libtool
AR_OPT = -static $^ -o $@
else
AR = ar
AR_OPT = rcs $@ $^
endif

CFLAGS += ${WARNINGS}

all: static dynamic demo

.PHONY:  static dynamic
static:  ${PRG}.a
dynamic: ${PRG}.so

${PRG}.a: ${OBJ}
	${AR} ${AR_OPT}

${PRG}.so: ${OBJ}
	${CC} -shared $< -o $@

.c.o:
	${CC} ${CFLAGS} $< -c -o ${<:.c=.o}

clean:
	rm -rf ${PRG}.a ${PRG}.so ${OBJ}
