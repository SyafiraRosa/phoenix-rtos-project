#include <iostream>
#include <vector>
#include <string>
#include "semaphore.h"

int const threadsCounts = 4;  // number of threads

int const bufferSize = 9;


class Buffer
{
private:

	std::vector<int> values;
	Semaphore mutex;
	bool readA, readB;

	void print(std::string name)
	{
		std::cout << "\n### " << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "]###\n";
	}

public:
	Buffer() : mutex(1), readA(false), readB(false)
	{
	}

	void put(int value)
	{
		mutex.p();
		// insert element to buffer
		values.push_back(value);
		print("P");
		mutex.v();
	}

	int getA()
	{
		mutex.p();
		// read element from buffer (without removing)
		int v = values.front();
		readA = true;
		print("A read");
		if (readB)
		{
			// remove element from buffer
			values.erase(values.begin());
			print("A remove");
			readA = readB = false;
		}
		mutex.v();
		return v;
	}

	int getB()
	{
		mutex.p();
		// read element from buffer (without removing)
		int v = values.front();
		readB = true;
		print("B read");
		if (readA)
		{
			// remove element from buffer
			values.erase(values.begin());
			print("B remove");
			readA = readB = false;
		}
		mutex.v();
		return v;
	}
};

Buffer buffer;

void* threadProd(void* arg)
{
	for (int i = 0; i < 10; ++i)
	{
		buffer.put(i);
	}
	return NULL;
}

void* threadConsA(void* arg)
{
	for (int i = 0; i < 17; ++i)
	{
		auto value = buffer.getA();
	}
	return NULL;
}

void* threadConsB(void* arg)
{
	for (int i = 0; i < 17; ++i)
	{
		auto value = buffer.getB();
	}
	return NULL;
}

int main()
{
#ifdef _WIN32
	HANDLE tid[threadsCounts];
	DWORD id;

	// create threads
	tid[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProd, 0, 0, &id);
	tid[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProd, 0, 0, &id);
	tid[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsA, 0, 0, &id);
	tid[3] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsB, 0, 0, &id);

	// wait for threads to finish
	for (int i = 0; i <= threadsCounts; i++)
		WaitForSingleObject(tid[i], INFINITE);
#else
	pthread_t tid[threadsCounts];

	// create threads
	pthread_create(&(tid[0]), NULL, threadProd, NULL);
	pthread_create(&(tid[1]), NULL, threadProd, NULL);
	pthread_create(&(tid[2]), NULL, threadConsA, NULL);
	pthread_create(&(tid[3]), NULL, threadConsB, NULL);

	// wait for threads to finish
	for (int i = 0; i < threadsCounts; i++)
		pthread_join(tid[i], (void**)NULL);
#endif
	return 0;
}
