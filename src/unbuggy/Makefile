CXX = clang++
CPPFLAGS = -I..
CXXFLAGS = -std=c++11 -pedantic -Wall -stdlib=libc++ -fpic

.PHONY: clean

libunbuggy.so: info_allocator.o
	$(CXX) -o $@ -shared $^

%.o: %.cpp
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	rm -f libunbuggy.so info_allocator.o
