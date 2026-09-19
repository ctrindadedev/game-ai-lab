MAKEFLAGS += --no-print-directory

BUILD_DIR := build
TARGET    := $(BUILD_DIR)/game
LEVEL     := assets/levels/level01.json

.PHONY: all build run level clean rebuild help

all: build

$(BUILD_DIR)/Makefile: CMakeLists.txt
	cmake -S . -B $(BUILD_DIR)

build: $(BUILD_DIR)/Makefile
	cmake --build $(BUILD_DIR) -j

run: build
	./$(TARGET)

level: build
	./$(TARGET) $(LEVEL)

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

help:
	@echo "make          configura e compila"
	@echo "make run      compila e executa com a configuracao padrao"
	@echo "make level    compila e executa lendo $(LEVEL)"
	@echo "make clean    apaga $(BUILD_DIR)/"
	@echo "make rebuild  apaga tudo e compila do zero"
