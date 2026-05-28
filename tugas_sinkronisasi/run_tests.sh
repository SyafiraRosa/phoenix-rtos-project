#!/bin/bash
echo "Compiling Synchronization Assignment on Unix/Linux..."

g++ semaphore_impl.cpp -o semaphore_impl -pthread
if [ $? -ne 0 ]; then
    echo "[ERROR] Semaphore compilation failed."
    exit 1
fi

g++ monitor_impl.cpp -o monitor_impl -pthread
if [ $? -ne 0 ]; then
    echo "[ERROR] Monitor compilation failed."
    exit 1
fi

echo -e "\nRunning Semaphore Tests..."
./semaphore_impl

echo -e "\nRunning Monitor Tests..."
./monitor_impl

echo -e "\nAll tests finished."
