FILES := $(shell find src include standalone \( -name "*.cpp" -o -name "*.h" \))

.PHONY: FORCE

send:
	git clone --recurse-submodules . luxora
	zip -r luxora.zip luxora
	rm -rf luxora

build: FORCE
	cmake -B build -S .
	cmake --build build
	./build/luxora-cli

test: FORCE
	cmake -B build -S .
	cmake --build build
	./build/tests/luxora-testing

format:
	clang-format -i $(FILES)
