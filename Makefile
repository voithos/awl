CC = cc
CFLAGS = -std=c99 -Wall 
LDFLAGS = -ledit -lm

TARGET = awl
SOURCES = $(wildcard src/*.c)
OBJECTS = $(addprefix obj/, $(notdir $(SOURCES:.c=.o)))

all: $(TARGET)

debug: CFLAGS += -g
debug: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(TARGET) obj/*.o
