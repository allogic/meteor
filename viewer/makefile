VER_MAJOR = 0
VER_MINOR = 1
VER_PATCH = 0

GIT_COMMIT_HASH = $(shell git rev-parse HEAD)

TARGET = viewer

WINDOW_NAME = "$(TARGET) $(VER_MAJOR).$(VER_MINOR).$(VER_PATCH) HEAD $(GIT_COMMIT_HASH)"

LIBRARY_PATH = ../library
PLATFORM_PATH = $(LIBRARY_PATH)/platform

CC = clang
LD = clang

CFLAGS += -std=c99
CFLAGS += -pedantic
CFLAGS += -g
CFLAGS += -c

CFLAGS += -Wall
CFLAGS += -Wextra

CFLAGS += -Wno-visibility

CFLAGS += -DWINDOW_NAME=\"$(WINDOW_NAME)\"

CFLAGS += -I$(LIBRARY_PATH)
CFLAGS += -I$(PLATFORM_PATH)

LFLAGS += -Wl,-Map=$(TARGET).map

SOURCES += main.c

SOURCES += $(LIBRARY_PATH)/window.c
SOURCES += $(LIBRARY_PATH)/list.c
SOURCES += $(LIBRARY_PATH)/fs.c

SOURCES += $(PLATFORM_PATH)/glad.c

ifneq (, $(filter windows, $(MAKECMDGOALS)))
CFLAGS += -DOS_WINDOWS

LFLAGS += -mwindows
endif

ifneq (, $(filter linux, $(MAKECMDGOALS)))
SOURCES += $(PLATFORM_PATH)/xdgshell.c

CFLAGS += -DOS_LINUX

LFLAGS += -lwayland-client
LFLAGS += -lwayland-egl
LFLAGS += -lEGL
endif

.PRECIOUS: $(OBJECTS)

OBJECTS += $(SOURCES:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.lnk: $(OBJECTS)
	$(LD) $(LFLAGS) -m64 -o $@ $(OBJECTS)

windows: $(TARGET).lnk
	mv $(TARGET).lnk $(TARGET).exe

linux: $(TARGET).lnk
	mv $(TARGET).lnk $(TARGET).elf

clean:
	rm -f $(OBJECTS) *.lnk *.exe *.elf *.map