FILES := $(shell find src include standalone \( -name "*.cpp" -o -name "*.h" \))
BUILD := build
TESTS := build/tests

EXEC := luxora-cli
DTEST := debug-test
TEST_EXEC := luxora-testing

.PHONY: FORCE

send:
	git clone --recurse-submodules . luxora
	zip -r luxora.zip luxora
	rm -rf luxora

build: FORCE
	cmake -B build -S .
	cmake --build ${BUILD} --target ${EXEC}

run: build
	./${BUILD}/${EXEC} $(filter-out $@,$(MAKECMDGOALS))

debug-test: FORCE
	cmake -B ${BUILD} -S .
	cmake --build ${BUILD} --target ${DTEST}
	./${TESTS}/${DTEST}

test: FORCE
	cmake -B build -S .
	cmake --build ${BUILD} --target ${TEST_EXEC}
	./${TESTS}/${TEST_EXEC}

format:
	clang-format -i $(FILES)

clean:
	rm -rf ${BUILD}
