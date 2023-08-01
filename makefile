VER_MAJOR = 0
VER_MINOR = 1
VER_PATCH = 0

GIT_COMMIT_HASH = $(shell git rev-parse HEAD)

SRC = src

CC = clang
LD = clang

CFLAGS += -std=c99
CFLAGS += -pedantic
CFLAGS += -g
CFLAGS += -c

CFLAGS += -Wall
CFLAGS += -Wextra

CFLAGS += -Wno-visibility

CFLAGS += -DVER_MAJOR="$(VER_MAJOR)"
CFLAGS += -DVER_MINOR="$(VER_MINOR)"
CFLAGS += -DVER_PATCH="$(VER_PATCH)"
CFLAGS += -DGIT_COMMIT_HASH="$(GIT_COMMIT_HASH)"

LFLAGS += -Wl,-Map=$@.map

SOURCES += $(SRC)/main.c
SOURCES += $(SRC)/window.c
SOURCES += $(SRC)/glad.c
SOURCES += $(SRC)/list.c
SOURCES += $(SRC)/fs.c

OBJECTS += $(SOURCES:.c=.o)

.PRECIOUS: $(OBJECTS)

ifeq ($(PLATFORM), WINDOWS)
CFLAGS += -DOS_WINDOWS

LFLAGS += -mwindows
endif

ifeq ($(PLATFORM), LINUX)
CFLAGS += -DOS_LINUX

LFLAGS += -lwayland-client
LFLAGS += -lwayland-egl
LFLAGS += -lEGL
endif

$(SRC)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -o $@ $<

%.exe: $(OBJECTS)
	$(LD) $(LFLAGS) -m64 -o $@ $(OBJECTS)

%.elf: $(OBJECTS)
	$(LD) $(LFLAGS) -m64 -o $@ $(OBJECTS)

all:

clean:
	rm -f $(SRC)/*.o *.exe *.elf *.map