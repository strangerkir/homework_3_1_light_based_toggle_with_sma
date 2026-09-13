
.PHONY: format build flash monitor bfm

format:
		find main -type f \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} +

build:
	idf.py build

flash:
	idf.py flash

monitor:
	idf.py monitor

bfm:
	idf.py build flash monitor
