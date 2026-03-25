build:
	cmake -S . -B build
	cmake --build build

run: build
	./build/vestify

clean:
	rm -rf build