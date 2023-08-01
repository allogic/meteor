SRC := src

CC := clang
LD := clang

CFLAGS += -std=c99
CFLAGS += -pedantic
CFLAGS += -g
CFLAGS += -c

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wno-visibility

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

LFLAGS += -lopengl32
LFLAGS += -lgdi32
LFLAGS += -lglu32
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

clean:
	rm -f $(SRC)/*.o *.exe *.elf *.map