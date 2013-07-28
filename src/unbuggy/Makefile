CXX = clang++
CPPFLAGS = -I..
CXXFLAGS = -std=c++11 -pedantic -Wall -stdlib=libc++ -fpic
LDFLAGS = -L. -lunbuggy -stdlib=libc++

.PHONY: clean default test

default: test

clean:
	rm -f libunbuggy.so info_allocator.o info_allocator_test

test: info_allocator_test
	DYLD_LIBRARY_PATH=. ./info_allocator_test

libunbuggy.so: info_allocator.o
	$(CXX) -o $@ -shared $^

info_allocator_test: info_allocator_test.o info_allocator.o
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp %.hpp
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) -c $<
