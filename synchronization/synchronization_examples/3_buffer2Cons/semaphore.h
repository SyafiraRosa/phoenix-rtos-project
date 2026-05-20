#ifndef __semaphore_h
#define __semaphore_h

#include <stdio.h> 
#include <stdlib.h> 

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h> 
#include <sys/stat.h> 
#include <string.h> 
#include <errno.h> 
#include <fcntl.h> 
#include <pthread.h> 
#include <unistd.h>
#include <semaphore.h>

#endif

class Semaphore
{
public:

  Semaphore( int value )
  {
#ifdef _WIN32
	sem = CreateSemaphore( NULL, value, 100, NULL );
#else
     if( sem_init( & sem, 0, value ) != 0 )
       throw "sem_init: failed";
#endif
  }
  ~Semaphore()
  { 
#ifdef _WIN32
	CloseHandle( sem );
#else
	  sem_destroy( & sem ); 
#endif
  }

  void p()
  {
#ifdef _WIN32
	  WaitForSingleObject( sem, INFINITE );
#else
     if( sem_wait( & sem ) != 0 )
       throw "sem_wait: failed";
#endif
  }

  void v()
  {
#ifdef _WIN32
	  ReleaseSemaphore( sem, 1, NULL );
#else
     if( sem_post( & sem ) != 0 )
       throw "sem_post: failed";
#endif
  }
  

private:

#ifdef _WIN32
	HANDLE sem;
#else
	sem_t sem;
#endif
};

#endif

