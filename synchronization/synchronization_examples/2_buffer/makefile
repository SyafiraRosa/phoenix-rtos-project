all: buffer bufferResolved

clean:
	rm -f buffer
	rm -f bufferResolved

buffer: buffer.cpp
	g++ buffer.cpp -lpthread -o buffer

bufferResolved: bufferResolved.cpp
	g++ bufferResolved.cpp -lpthread -o bufferResolved
