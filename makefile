SRC := src
OBJ := obj

CC := clang
LD := clang

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

CFLAGS := -std=c99 -pedantic -Wall -Wextra -g -c
LDFLAGS = -mwindows -lopengl32 -lgdi32 -lglu32

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -o $@ $<

%.exe: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

all: main.exe

clean:
	rm -f $(OBJ)/*.o *.exe