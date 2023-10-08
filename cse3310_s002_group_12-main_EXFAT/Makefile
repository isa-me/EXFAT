# for this project, there is only one makefile
#
# this makefile is expected to be run from the project 
# root.
#
# all executables are expected to be ran from the project 
# root.
#
# this makefile will clean up when asked politely
#
# No intermediate .o files or libraries are created

CPPFLAGS=-Wall -Wextra -O0 -std=c17 -g3 -fsanitize=address -fsanitize=bounds-strict
# added -fsanitize=bounds-strict

# note address sanitizer "-fsanitize=address" is new. it can be
# removed from the makefile if it causes problems.

CPPINCS=-I./include

all:mmap fread unit_tests extfat

# the utility that is a focus of this project
extfat:src/extfat.c common/routines.c src/processArgs.c src/fileMapping.c src/verify.c src/printDirectory.c src/removeFile.c src/imageFileHandling.c
	${CC} ${CPPFLAGS} ${CPPINCS} -o $@ $^



# unit tests
unit_tests: munit

# this test needs to be deleted once we get some real tests
# for the problem at hand
munit:unit_tests/munit/unit.c
	${CC} ${CPPFLAGS}  unit_tests/munit/munit.c -I./unit_tests/munit/ ${CPPINCS} -o $@ $^

# requirements tests


# example code
mmap:examples/mmap.c  
	${CC} ${CPPFLAGS} ${CPPINCS} -o $@ $^

fread:examples/fread.c  
	${CC} ${CPPFLAGS} ${CPPINCS} -o $@ $^


# run tests
tests: run_unit_tests run_tests
	bash tests/system_tests.bash
run_unit_tests: munit
	bash tests/make_example_image.bash
	./munit
	rm copy.image
	bash tests/clean_up.bash

run_tests:
	echo "here i would be running the requirements tests"

clean:
	-rm -f mmap fread munit extfat crc_example munit_example

system_tests: extfat
	bash tests/system_tests.bash