.PHONY: test install-hooks

all: exercises

exercises:
	./scripts/build.sh

test:
	./scripts/tests.sh

install-hooks:
	./scripts/install-hooks.sh

clean:
	rm -rf build/

cppcheck:
	cppcheck --xml --std=c++14 --enable=all -I./include -I./thirdparty src/ 2> cppcheck.xml

memcheck:
	./scripts/memcheck.sh
