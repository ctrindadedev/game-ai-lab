MAKEFLAGS += --no-print-directory

BUILD_DIR := build
TARGET    := $(BUILD_DIR)/game
LEVEL     := assets/levels/level01.json

.PHONY: all build run level random benchmark clean rebuild help

all: build

$(BUILD_DIR)/Makefile: CMakeLists.txt
	cmake -S . -B $(BUILD_DIR)

build: $(BUILD_DIR)/Makefile
	cmake --build $(BUILD_DIR) -j

run: build
	./$(TARGET)

level: build
	./$(TARGET) $(LEVEL)

random: build
	./$(TARGET) --random 1

benchmark: build
	./$(TARGET) --benchmark benchmarks/frame_time.csv

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

help:
	@echo "make            configura e compila"
	@echo "make run        compila e executa com a configuracao padrao"
	@echo "make level      compila e executa lendo $(LEVEL)"
	@echo "make random     compila e executa com nivel gerado aleatoriamente"
	@echo "make benchmark  compila e mede tempo de quadro (com/sem janela ativa) em benchmarks/frame_time.csv"
	@echo "make clean      apaga $(BUILD_DIR)/"
	@echo "make rebuild    apaga tudo e compila do zero"
