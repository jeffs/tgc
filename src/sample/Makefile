CXX = clang++
CPPFLAGS = -I..
CXXFLAGS = -std=c++11 -pedantic -Wall -stdlib=libc++
LDFLAGS = -L../unbuggy -lunbuggy -stdlib=libc++

.PHONY: clean

main: main.o
	$(CXX) -o $@ $< $(LDFLAGS)

clean:
	rm -f main main.o
