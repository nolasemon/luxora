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
