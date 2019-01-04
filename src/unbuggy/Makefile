CXX = clang++
CPPFLAGS = -I..
CXXFLAGS = -std=c++11 -pedantic -Wall -stdlib=libc++
LDFLAGS = -stdlib=libc++

.PHONY: clean doc test test_level1 test_level2 test_usage

.PHONY: site

site: site1 site2

test: test_level1 test_level2 test_usage

# Level 1

test_level1: \
    counting_allocator_test \
    null_allocator_delegate_test
	./counting_allocator_test
	./null_allocator_delegate_test

counting_allocator.o: \
    counting_allocator.cpp \
    counting_allocator.hpp
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) -c $<

counting_allocator_test: \
    counting_allocator_test.o \
    counting_allocator.o \
    delegate_allocator.o
	$(CXX) -o $@ $^ $(LDFLAGS)

null_allocator_delegate_test: \
    null_allocator_delegate_test.o \
    null_allocator_delegate.o
	$(CXX) -o $@ $^ $(LDFLAGS)

# Level 2

test_level2: delegate_allocator_test
	./delegate_allocator_test

delegate_allocator_test: \
    delegate_allocator_test.o \
    delegate_allocator.o \
    null_allocator_delegate.o
	$(CXX) -o $@ $^ $(LDFLAGS)

# Usage

test_usage: usage
	./usage

site%: site%.cpp
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) $^ $(LDFLAGS)

usage: usage.o counting_allocator.o delegate_allocator.o
	$(CXX) -o $@ $^ $(LDFLAGS)

usage.o: usage.cpp fml.hpp scoped.hpp counting_allocator.hpp
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) -c $<

# Rules

%.o: %.cpp %.hpp
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) -c $<

%_test.o: %_test.cpp %.hpp %.tpp
	$(CXX) -o $@ $(CPPFLAGS) $(CXXFLAGS) -c $<

clean:
	rm -f *.o *_test

doc:
	doxygen Doxyfile
