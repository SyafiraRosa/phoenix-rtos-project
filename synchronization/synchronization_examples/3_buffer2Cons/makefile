all: buffer2cons buffer2consResolved buffer2consDirectResume buffer2consDirectResume2

clean:
	rm -f buffer2cons
	rm -f buffer2consResolved
	rm -f buffer2consDirectResume
	rm -f buffer2consDirectResume2

buffer2cons: buffer2cons.cpp
	g++ buffer2cons.cpp -lpthread -o buffer2cons

buffer2consResolved: buffer2consResolved.cpp
	g++ buffer2consResolved.cpp -lpthread -o buffer2consResolved
	
buffer2consDirectResume: buffer2consDirectResume.cpp
	g++ buffer2consDirectResume.cpp -lpthread -o buffer2consDirectResume

buffer2consDirectResume2: buffer2consDirectResume2.cpp
	g++ buffer2consDirectResume2.cpp -lpthread -o buffer2consDirectResume2
