CXX = clang++
CPPFLAGS = -I..
CXXFLAGS = -std=c++11 -pedantic -Wall -stdlib=libc++
LDFLAGS = -stdlib=libc++

.PHONY: clean doc test

test: info_allocator_test usage
	./info_allocator_test
	./usage < Makefile >/dev/null

info_allocator_test: info_allocator_test.o info_allocator.o
	$(CXX) -o $@ $^ $(LDFLAGS)

usage: usage.o info_allocator.o
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp %.hpp
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) -c $<

%_test.o: %_test.cpp %.hpp %.tpp
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	rm -f *.o *_test

doc:
	doxygen Doxyfile
