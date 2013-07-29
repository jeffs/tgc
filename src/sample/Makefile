CXX = clang++
CPPFLAGS = -I..
CXXFLAGS = -std=c++11 -pedantic -Wall -stdlib=libc++
LDFLAGS = -L../unbuggy -lunbuggy -stdlib=libc++

.PHONY: clean default run

default: run

clean:
	rm -f main main.o

run: main
	@DYLD_LIBRARY_PATH=../unbuggy ./main

main: main.o
	$(CXX) -o $@ $< $(LDFLAGS)

