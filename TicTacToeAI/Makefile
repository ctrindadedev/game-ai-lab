MAKEFLAGS += --no-print-directory

BUILD_DIR := build
TARGET    := $(BUILD_DIR)/tictactoe

.PHONY: all build run clean rebuild help

all: build

$(BUILD_DIR)/Makefile: CMakeLists.txt
	cmake -S . -B $(BUILD_DIR)

build: $(BUILD_DIR)/Makefile
	cmake --build $(BUILD_DIR) -j

run: build
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

help:
	@echo "make            configura e compila"
	@echo "make run        compila e executa"
	@echo "make clean      apaga $(BUILD_DIR)/"
	@echo "make rebuild    apaga tudo e compila do zero"
