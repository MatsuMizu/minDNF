CC = g++

CPPFLAGS = -DM=10 -O3 -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format



all: a.out

a.out: minDNF.o
	$(CC) -g $(CFLAGS) $^ -lm -o $@

minDNF.o: minDNF.cpp methods.h list2.h condition.h
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.out *.o
	rm -f data.h.gch a.out data.o