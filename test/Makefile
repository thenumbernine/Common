.PHONY: all
all: meta.out

meta.out: meta.cpp
	clang++ -std=c++11 -Wall -I../include meta.cpp -o meta.out

.PHONY: clean
clean:
	-rm meta.out
