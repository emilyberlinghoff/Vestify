.PHONY: build run clean

build:
	cmake -S . -B build
	cmake --build build

run: build
	find build -type f -executable

clean:
	rm -rf build