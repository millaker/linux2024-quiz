CC = gcc
CFLAGS = -Wall -g
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
TARGET = sort

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MF .$*.d -c $<

.PHONY: clean
clean:
	rm -rf $(OBJ) $(TARGET)