#include "monitor.h"
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

class MyMonitor : public Monitor {
public:
    void putEven(int element, int id) {
        enter();
        while (!canProdEven(buffer)) {
            ++numOfProdEvenWaiting;
            wait(prodEvenCond);
            --numOfProdEvenWaiting;
        }
        buffer.push(element);
        cout << "[A1-" << id << "] Produced Even: " << element << " | " << buffer.print() << endl;
        releaseBaton();
        leave();
    }

    void putOdd(int element, int id) {
        enter();
        while (!canProdOdd(buffer)) {
            ++numOfProdOddWaiting;
            wait(prodOddCond);
            --numOfProdOddWaiting;
        }
        buffer.push(element);
        cout << "[A2-" << id << "] Produced Odd:  " << element << " | " << buffer.print() << endl;
        releaseBaton();
        leave();
    }

    int getEven(int id) {
        enter();
        while (!canConsEven(buffer)) {
            ++numOfConsEvenWaiting;
            wait(consEvenCond);
            --numOfConsEvenWaiting;
        }
        int element = buffer.popEven();
        cout << "[B1-" << id << "] Consumed Even: " << element << " | " << buffer.print() << endl;
        releaseBaton();
        leave();
        return element;
    }

    int getOdd(int id) {
        enter();
        while (!canConsOdd(buffer)) {
            ++numOfConsOddWaiting;
            wait(consOddCond);
            --numOfConsOddWaiting;
        }
        int element = buffer.popOdd();
        cout << "[B2-" << id << "] Consumed Odd:  " << element << " | " << buffer.print() << endl;
        releaseBaton();
        leave();
        return element;
    }

    void reset() {
        enter();
        buffer = Buffer();
        numOfProdEvenWaiting = 0;
        numOfProdOddWaiting = 0;
        numOfConsEvenWaiting = 0;
        numOfConsOddWaiting = 0;
        leave();
    }

    void wakeupAll() {
        enter();
        // Force signals to wake up anyone waiting to check 'running' flag
        signal(prodEvenCond);
        signal(prodOddCond);
        signal(consEvenCond);
        signal(consOddCond);
        leave();
    }

private:
    void releaseBaton() {
        if (numOfProdEvenWaiting > 0U && canProdEven(buffer)) {
            signal(prodEvenCond);
        } else if (numOfProdOddWaiting > 0U && canProdOdd(buffer)) {
            signal(prodOddCond);
        } else if (numOfConsEvenWaiting > 0U && canConsEven(buffer)) {
            signal(consEvenCond);
        } else if (numOfConsOddWaiting > 0U && canConsOdd(buffer)) {
            signal(consOddCond);
        }
    }

    Buffer buffer;
    Condition prodEvenCond, prodOddCond, consEvenCond, consOddCond;
    unsigned int numOfProdEvenWaiting = 0U, numOfProdOddWaiting = 0U, numOfConsEvenWaiting = 0U, numOfConsOddWaiting = 0U;
};

MyMonitor myMonitor;
bool running = true;

// Generators
int nextEven = 0;
int nextOdd = 1;

int generateNextEvenNumber() {
    int val = nextEven;
    nextEven = (nextEven + 2) % 50;
    return val;
}

int generateNextOddNumber() {
    int val = nextOdd;
    nextOdd = (nextOdd + 2) % 50;
    return val;
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
    while(running) {
        int element = generateNextEvenNumber();
        myMonitor.putEven(element, id);
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
    while(running) {
        int element = generateNextOddNumber();
        myMonitor.putOdd(element, id);
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
    while(running) {
        myMonitor.getEven(id);
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
    while(running) {
        myMonitor.getOdd(id);
#ifdef _WIN32
        Sleep(rand() % 100 + 50);
#else
        usleep((rand() % 100 + 50) * 1000);
#endif
    }
    THREAD_RET;
}

void runTestCase(int nA1, int nA2, int nB1, int nB2) {
    cout << "\n>>> Starting Monitor Test: A1=" << nA1 << ", A2=" << nA2 << ", B1=" << nB1 << ", B2=" << nB2 << " <<<\n";
    running = true;
    myMonitor.reset();
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
    myMonitor.wakeupAll();

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
    myMonitor.wakeupAll();

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
