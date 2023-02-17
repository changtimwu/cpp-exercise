all: test

gptex: gptex.cpp
	g++ $< -o $@

test: test.cpp
	g++ $< -o $@

ex: ex.cpp
	g++ $< -o $@


