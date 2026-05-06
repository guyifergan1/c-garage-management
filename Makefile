# Cross-platform Makefile for the Vehicle Service Management System.
# Works with GCC/Clang on Linux/macOS/MinGW. On Windows + MSVC, see README.

CC      ?= gcc
CFLAGS  ?= -std=c11 -Wall -Wextra -O2
TARGET  := garage
SRC     := main.c

ifeq ($(OS),Windows_NT)
    BIN := $(TARGET).exe
    RM  := del /Q
else
    BIN := $(TARGET)
    RM  := rm -f
endif

.PHONY: all clean run

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

run: $(BIN)
	./$(BIN)

clean:
	-$(RM) $(BIN) *.o *.obj log.txt Employee.txt clients.txt \
	       items.bin inventory.bin sales.bin temp_*.bin temp_*.txt Temp.txt
