#include <iostream>
#include <string>
#include "semaphore.h"

int const threadsCounts = 2;  // number of threads

int const totalNumberOfLettersA = 20;
int const totalNumberOfLettersB = 20;

std::string s;

Semaphore sem(1);
Semaphore semA(1);
Semaphore semB(0);

void writeA()
{
	semA.p();
	sem.p();
	std::cout << "A" << std::flush;
	s += "A";
	sem.v();
	semB.v();
}

void writeB()
{
	semB.p();
	sem.p();
	std::cout << "B" << std::flush;
	s += "B";
	sem.v();
	semA.v();
}

void* threadA(void* arg)
{
	for (int i = 0; i < totalNumberOfLettersA; ++i)
	{
		writeA();
	}
	return NULL;
}

void* threadB(void* arg)
{
	for (int i = 0; i < totalNumberOfLettersB; ++i)
	{
		writeB();
	}
	return NULL;
}

int main()
{
	std::cout << "  " << std::flush;
#ifdef _WIN32
	HANDLE tid[threadsCounts];
	DWORD id;

	// create threads
	tid[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadA, 0, 0, &id);
	tid[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadB, 0, 0, &id);

	// wait for threads to finish
	for (int i = 0; i <= threadsCounts; i++)
		WaitForSingleObject(tid[i], INFINITE);
#else
	pthread_t tid[threadsCounts];

	// create threads
	pthread_create(&(tid[0]), NULL, threadA, NULL);
	pthread_create(&(tid[1]), NULL, threadB, NULL);

	// wait for threads to finish
	for (int i = 0; i < threadsCounts; i++)
		pthread_join(tid[i], (void**)NULL);
#endif
	std::cout << "\n";
	std::cout << "s=" << s << "\n";
	return 0;
}
