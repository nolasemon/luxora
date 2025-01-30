FILES := $(shell find src include standalone tests \( -name "*.cpp" -o -name "*.h" \))
BUILD := build
TESTS := build/tests

EXEC := luxora-cli
DTEST := debug-test
TEST_EXEC := luxora-testing

.PHONY: FORCE

default: build

send:
	git clone --recurse-submodules . luxora
	cp -r site luxora/luxora
	cd luxora; rm -rf .git external/**/tests external/**/docs external/**/doc; make build; make test; rm -rf build
	zip -r luxora.zip luxora
	rm -rf luxora

build: FORCE
	cmake -B ${BUILD} -S .
	cmake --build ${BUILD} --target ${EXEC}

run: build
	./${BUILD}/${EXEC}

debug-test: FORCE
	cmake -B ${BUILD} -S .
	cmake --build ${BUILD} --target ${DTEST}
	./${TESTS}/${DTEST}

test: FORCE
	cmake -B ${BUILD} -S .
	cmake --build ${BUILD} --target ${TEST_EXEC}
	ctest --test-dir ${BUILD}

format:
	clang-format -i $(FILES)

clean:
	rm -rf ${BUILD}

docs:
	find build/ -name *.gcda | xargs gcov --stdout > build/coverage.gcov
	doxide build
	mkdocs build
