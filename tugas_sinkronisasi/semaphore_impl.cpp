#include "semaphore.h"
#include "buffer.h"
#include <iostream>
#include <vector>
#include <string>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

using namespace std;

// Synchronization primitives (using lecturer's Semaphore class)
Semaphore mutex_sem(1);
Semaphore prodEvenSem(0), prodOddSem(0), consEvenSem(0), consOddSem(0);
unsigned int numOfProdEvenWaiting = 0U, numOfProdOddWaiting = 0U, numOfConsEvenWaiting = 0U, numOfConsOddWaiting = 0U;

Buffer buffer;

// Shared state for generation
int nextEven = 0;
int nextOdd = 1;
bool running = true;

int generateEvenNumber() {
    int val = nextEven;
    nextEven = (nextEven + 2) % 50;
    return val;
}

int generateOddNumber() {
    int val = nextOdd;
    nextOdd = (nextOdd + 2) % 50;
    return val;
}

void releaseBaton() {
    if (numOfProdEvenWaiting > 0U && canProdEven(buffer)) {
        prodEvenSem.v();
    } else if (numOfProdOddWaiting > 0U && canProdOdd(buffer)) {
        prodOddSem.v();
    } else if (numOfConsEvenWaiting > 0U && canConsEven(buffer)) {
        consEvenSem.v();
    } else if (numOfConsOddWaiting > 0U && canConsOdd(buffer)) {
        consOddSem.v();
    } else {
        mutex_sem.v();
    }
}

#ifdef _WIN32
#define THREAD_FUNC_DECL DWORD WINAPI
#define THREAD_RET return 0
typedef LPVOID THREAD_PARAM;
#else
#define THREAD_FUNC_DECL void*
#define THREAD_RET return NULL
typedef void* THREAD_PARAM;
#endif

THREAD_FUNC_DECL producerEvenFunc(THREAD_PARAM arg) {
    int id = (int)(size_t)arg;
    while (running) {
        mutex_sem.p();
        while (!canProdEven(buffer)) {
            ++numOfProdEvenWaiting;
            mutex_sem.v();
            prodEvenSem.p();
            --numOfProdEvenWaiting;
        }
        if (!running) { releaseBaton(); THREAD_RET; }

        int element = generateEvenNumber();
        buffer.push(element);
        cout << "[A1-" << id << "] Produced Even: " << element << " | " << buffer.print() << endl;
        
        releaseBaton();
#ifdef _WIN32
        Sleep(rand() % 100 + 50);
#else
        usleep((rand() % 100 + 50) * 1000);
#endif
    }
    THREAD_RET;
}

THREAD_FUNC_DECL producerOddFunc(THREAD_PARAM arg) {
    int id = (int)(size_t)arg;
    while (running) {
        mutex_sem.p();
        while (!canProdOdd(buffer)) {
            ++numOfProdOddWaiting;
            mutex_sem.v();
            prodOddSem.p();
            --numOfProdOddWaiting;
        }
        if (!running) { releaseBaton(); THREAD_RET; }

        int element = generateOddNumber();
        buffer.push(element);
        cout << "[A2-" << id << "] Produced Odd:  " << element << " | " << buffer.print() << endl;
        
        releaseBaton();
#ifdef _WIN32
        Sleep(rand() % 100 + 50);
#else
        usleep((rand() % 100 + 50) * 1000);
#endif
    }
    THREAD_RET;
}

THREAD_FUNC_DECL consumerEvenFunc(THREAD_PARAM arg) {
    int id = (int)(size_t)arg;
    while (running) {
        mutex_sem.p();
        while (!canConsEven(buffer)) {
            ++numOfConsEvenWaiting;
            mutex_sem.v();
            consEvenSem.p();
            --numOfConsEvenWaiting;
        }
        if (!running) { releaseBaton(); THREAD_RET; }

        int element = buffer.popEven();
        cout << "[B1-" << id << "] Consumed Even: " << element << " | " << buffer.print() << endl;
        
        releaseBaton();
#ifdef _WIN32
        Sleep(rand() % 100 + 50);
#else
        usleep((rand() % 100 + 50) * 1000);
#endif
    }
    THREAD_RET;
}

THREAD_FUNC_DECL consumerOddFunc(THREAD_PARAM arg) {
    int id = (int)(size_t)arg;
    while (running) {
        mutex_sem.p();
        while (!canConsOdd(buffer)) {
            ++numOfConsOddWaiting;
            mutex_sem.v();
            consOddSem.p();
            --numOfConsOddWaiting;
        }
        if (!running) { releaseBaton(); THREAD_RET; }

        int element = buffer.popOdd();
        cout << "[B2-" << id << "] Consumed Odd:  " << element << " | " << buffer.print() << endl;
        
        releaseBaton();
#ifdef _WIN32
        Sleep(rand() % 100 + 50);
#else
        usleep((rand() % 100 + 50) * 1000);
#endif
    }
    THREAD_RET;
}

void runTestCase(int nA1, int nA2, int nB1, int nB2) {
    cout << "\n>>> Starting Semaphore Test: A1=" << nA1 << ", A2=" << nA2 << ", B1=" << nB1 << ", B2=" << nB2 << " <<<\n";
    running = true;
    buffer = Buffer();
    nextEven = 0;
    nextOdd = 1;

#ifdef _WIN32
    vector<HANDLE> threads;
    DWORD tid;
    for (int i = 0; i < nA1; ++i) threads.push_back(CreateThread(NULL, 0, producerEvenFunc, (LPVOID)(size_t)(i + 1), 0, &tid));
    for (int i = 0; i < nA2; ++i) threads.push_back(CreateThread(NULL, 0, producerOddFunc, (LPVOID)(size_t)(i + 1), 0, &tid));
    for (int i = 0; i < nB1; ++i) threads.push_back(CreateThread(NULL, 0, consumerEvenFunc, (LPVOID)(size_t)(i + 1), 0, &tid));
    for (int i = 0; i < nB2; ++i) threads.push_back(CreateThread(NULL, 0, consumerOddFunc, (LPVOID)(size_t)(i + 1), 0, &tid));

    Sleep(5000);
    running = false;
    mutex_sem.p(); releaseBaton(); // Wake up anyone waiting

    for (auto h : threads) {
        WaitForSingleObject(h, INFINITE);
        CloseHandle(h);
    }
#else
    vector<pthread_t> threads(nA1 + nA2 + nB1 + nB2);
    int idx = 0;
    for (int i = 0; i < nA1; ++i) pthread_create(&threads[idx++], NULL, producerEvenFunc, (void*)(size_t)(i + 1));
    for (int i = 0; i < nA2; ++i) pthread_create(&threads[idx++], NULL, producerOddFunc, (void*)(size_t)(i + 1));
    for (int i = 0; i < nB1; ++i) pthread_create(&threads[idx++], NULL, consumerEvenFunc, (void*)(size_t)(i + 1));
    for (int i = 0; i < nB2; ++i) pthread_create(&threads[idx++], NULL, consumerOddFunc, (void*)(size_t)(i + 1));

    sleep(5);
    running = false;
    mutex_sem.p(); releaseBaton();

    for (auto t : threads) pthread_join(t, NULL);
#endif
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));

    if (argc == 5) {
        int a1 = atoi(argv[1]);
        int a2 = atoi(argv[2]);
        int b1 = atoi(argv[3]);
        int b2 = atoi(argv[4]);
        runTestCase(a1, a2, b1, b2);
        return 0;
    }

    // Default behavior if no arguments provided: run all tests
    runTestCase(1, 0, 0, 0);
    runTestCase(0, 1, 0, 0);
    runTestCase(0, 0, 1, 0);
    runTestCase(0, 0, 0, 1);
    runTestCase(1, 1, 1, 1);
    runTestCase(2, 2, 2, 2);

    return 0;
}
