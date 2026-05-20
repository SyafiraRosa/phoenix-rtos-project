#include <iostream>
#include <vector>
#include <string>
#include "semaphore.h"

int const threadsCounts = 4;  // number of threads

int const bufferSize = 9;


class Buffer
{
private:
	Semaphore mutex;
	Semaphore full;
	Semaphore stopA;
	Semaphore stopB;
	bool readA = false;
	bool readB = false;
	bool waitA = false;
	bool waitB = false;
	std::vector<int> values;

	void print(std::string name)
	{
		std::cout << "\n### " << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "]###\n";
	}

public:
	Buffer() : mutex(1), full(bufferSize), stopA(0), stopB(0)
	{
	}

	bool canA()
	{
		return values.size() > 3 && !readA;
	}

	bool canB()
	{
		return values.size() > 3 && !readB;
	}

	void put(int value)
	{
		full.p();
		mutex.p();
		// insert element to buffer
		values.push_back(value);
		print("P");
		// resume processes (passing critical section)
		if (canA() && waitA)
			stopA.v();
		else
			if (canB() && waitB)
				stopB.v();
			else
				mutex.v();
	}

	int getA()
	{
		mutex.p();
		// check if process can perform operation
		if (!canA())
		{
			waitA = true;
			print("A wait");
			mutex.v();
			// suspend process on dedicated semaphore
			stopA.p();
			waitA = false;
			print("A resume");
		}

		// read element from buffer (without removing)
		int v = values.front();
		readA = true;
		print("A read");
		// check if element should be removed
		if (readB)
		{
			// remove element from buffer
			values.erase(values.begin());
			print("A remove");
			readA = readB = false;
			full.v();
			// resume processes (passing critical section)
			if (canB() && waitB)
				stopB.v();
			else
				mutex.v();
		}
		else
		{
			// resume processes (passing critical section)
			if (canB() && waitB)
				stopB.v();
			else
				mutex.v();
		}
		return v;
	}

	int getB()
	{
		mutex.p();
		// check if process can perform operation
		if (!canB())
		{
			waitB = true;
			print("B wait");
			mutex.v();
			// suspend process on dedicated semaphore
			stopB.p();
			waitB = false;
			print("B resume");
		}

		// read element from buffer (without removing)
		int v = values.front();
		readB = true;
		print("B read");
		// check if element should be removed
		if (readA)
		{
			// remove element from buffer
			values.erase(values.begin());
			print("B remove");
			readA = readB = false;
			full.v();
			// resume processes (passing critical section)
			if (canA() && waitA)
				stopA.v();
			else
				mutex.v();
		}
		else
		{
			// resume processes (passing critical section)
			if (canA() && waitA)
				stopA.v();
			else
				mutex.v();
		}
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
	std::cout << "### P finish ###\n";
	return NULL;
}

void* threadConsA(void* arg)
{
	for (int i = 0; i < 17; ++i)
	{
		auto value = buffer.getA();
	}
	std::cout << "### A finish ###\n";
	return NULL;
}

void* threadConsB(void* arg)
{
	for (int i = 0; i < 17; ++i)
	{
		auto value = buffer.getB();
	}
	std::cout << "### B finish ###\n";
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
