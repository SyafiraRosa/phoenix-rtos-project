all: pingPong pingPongResolved pingPong2 pingPong2Ext1 pingPong3Resolved pingPong3Ext1Resolved pingPong3Ext2Resolved

clean:
	rm -f pingPong
	rm -f pingPongResolved
	rm -f pingPong2
	rm -f pingPong2Ext1
	rm -f pingPong3Resolved
	rm -f pingPong3Ext1Resolved
	rm -f pingPong3Ext2Resolved

pingPong: pingPong.cpp
	g++ pingPong.cpp -lpthread -o pingPong

pingPongResolved: pingPongResolved.cpp
	g++ pingPongResolved.cpp -lpthread -o pingPongResolved
	
pingPong2: pingPong2.cpp
	g++ pingPong2.cpp -lpthread -o pingPong2

pingPong2Ext1: pingPong2Ext1.cpp
	g++ pingPong2Ext1.cpp -lpthread -o pingPong2Ext1

pingPong3Resolved: pingPong3Resolved.cpp
	g++ pingPong3Resolved.cpp -lpthread -o pingPong3Resolved

pingPong3Ext1Resolved: pingPong3Ext1Resolved.cpp
	g++ pingPong3Ext1Resolved.cpp -lpthread -o pingPong3Ext1Resolved

pingPong3Ext2Resolved: pingPong3Ext2Resolved.cpp
	g++ pingPong3Ext2Resolved.cpp -lpthread -o pingPong3Ext2Resolved
