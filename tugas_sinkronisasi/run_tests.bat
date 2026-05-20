@echo off
echo Compiling Synchronization Assignment (using native Windows threads)...

g++ semaphore_impl.cpp -o semaphore_impl.exe
if %errorlevel% neq 0 (
    echo [ERROR] Semaphore compilation failed.
    pause
    exit /b %errorlevel%
)

g++ monitor_impl.cpp -o monitor_impl.exe
if %errorlevel% neq 0 (
    echo [ERROR] Monitor compilation failed.
    pause
    exit /b %errorlevel%
)

echo.
echo Running Semaphore Tests...
semaphore_impl.exe

echo.
echo Running Monitor Tests...
monitor_impl.exe

echo.
echo All tests finished.
pause
