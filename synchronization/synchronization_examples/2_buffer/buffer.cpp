#include <iostream>
#include <vector>
#include <string>
#include "semaphore.h"

int const threadsCounts = 5;  // number of threads

int const bufferSize = 5;


class Buffer
{
private:
	//Semaphore mutex;

	std::vector<int> values;

	void print(std::string name)
	{
		std::cout << "\n ###" << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "] ###\n";
	}

public:
	Buffer() //: mutex(0)
	{
	}

	void put(int value)
	{
		values.push_back(value);
		print("P");
	}

	int getA()
	{
		auto value = values.front();
		values.erase(values.begin());
		print("A");
		return value;
	}

	int getB()
	{
		auto value = values.front();
		values.erase(values.begin());
		print("B");
		return value;
	}
};

Buffer buffer;

void* threadProd(void* arg)
{
	for (int i = 0; i < 20; ++i)
	{
		buffer.put(i);
	}
	return NULL;
}

void* threadConsA(void* arg)
{
	for (int i = 0; i < 30; ++i)
	{
		auto value = buffer.getA();
	}
	return NULL;
}

void* threadConsB(void* arg)
{
	for (int i = 0; i < 30; ++i)
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
	tid[2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProd, 0, 0, &id);
	tid[3] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsA, 0, 0, &id);
	tid[4] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadConsB, 0, 0, &id);

	// wait for threads to finish
	for (int i = 0; i <= threadsCounts; i++)
		WaitForSingleObject(tid[i], INFINITE);
#else
	pthread_t tid[threadsCounts];

	// create threads
	pthread_create(&(tid[0]), NULL, threadProd, NULL);
	pthread_create(&(tid[1]), NULL, threadProd, NULL);
	pthread_create(&(tid[2]), NULL, threadProd, NULL);
	pthread_create(&(tid[3]), NULL, threadConsA, NULL);
	pthread_create(&(tid[4]), NULL, threadConsB, NULL);

	// wait for threads to finish
	for (int i = 0; i < threadsCounts; i++)
		pthread_join(tid[i], (void**)NULL);
#endif
	return 0;
}

