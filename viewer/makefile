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

SOURCES += $(LIBRARY_PATH)/debug.c
SOURCES += $(LIBRARY_PATH)/window.c
SOURCES += $(LIBRARY_PATH)/list.c
SOURCES += $(LIBRARY_PATH)/fs.c
SOURCES += $(LIBRARY_PATH)/strutl.c

SOURCES += $(PLATFORM_PATH)/glad.c

ifneq (, $(filter windows, $(MAKECMDGOALS)))
#CFLAGS += -gcodeview
#CFLAGS += --target=x86_64-windows

CFLAGS += -DOS_WINDOWS

LFLAGS += -mwindows
endif

ifneq (, $(filter linux, $(MAKECMDGOALS)))
SOURCES += $(PLATFORM_PATH)/xdgshell.c

#CFLAGS += -gcodeview
#CFLAGS += --target=x86_64-linux

CFLAGS += -DOS_LINUX

LFLAGS += -lwayland-client
LFLAGS += -lwayland-egl
LFLAGS += -lEGL
endif

.PRECIOUS: $(OBJECTS)

OBJECTS += $(SOURCES:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.bin: $(OBJECTS)
	$(LD) $(LFLAGS) -m64 -o $@ $(OBJECTS)

windows: $(TARGET).bin
	mv $(TARGET).bin $(TARGET).exe

linux: $(TARGET).bin
	mv $(TARGET).bin $(TARGET).elf

clean:
	rm -f $(OBJECTS) *.bin *.exe *.elf *.map *.pdb