/*
 * sync_semaphore/main.c
 *
 * FIFO Buffer Synchronization — Semaphore Implementation
 *
 * Problem:
 *   A1: produce consecutive even numbers (mod 50) if count_even < 10
 *   A2: produce consecutive odd  numbers (mod 50) if count_even > count_odd
 *   B1: consume even numbers  if total >= 3
 *   B2: consume odd  numbers  if total >= 7
 *
 * Approach:
 *   Since Phoenix-RTOS has no sem_t, we build our own Semaphore struct
 *   on top of pthread_mutex_t + pthread_cond_t. Then we use those
 *   semaphores to synchronize the four thread types.
 *
 * Semaphore pattern used (mutex + condition semaphores):
 *   - mutex_sem    : binary semaphore for mutual exclusion
 *   - sem_a1/a2/b1/b2 : each thread type waits on its own semaphore
 *   - wait_a1/a2/b1/b2: count of blocked threads per type
 *
 * After each produce/consume, signal_waiters() checks which thread
 * types can now proceed and unblocks one thread of that type.
 *
 * Author: SyafiraRosa — 348296
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* =========================================================
 * Semaphore implementation (no sem.h in Phoenix-RTOS)
 * ========================================================= */
typedef struct {
	int value;
	pthread_mutex_t mtx;
	pthread_cond_t  cnd;
} Semaphore;

static void sem_init(Semaphore *s, int initial_value)
{
	s->value = initial_value;
	pthread_mutex_init(&s->mtx, NULL);
	pthread_cond_init(&s->cnd, NULL);
}

static void sem_wait_s(Semaphore *s)
{
	pthread_mutex_lock(&s->mtx);
	while (s->value <= 0)
		pthread_cond_wait(&s->cnd, &s->mtx);
	s->value--;
	pthread_mutex_unlock(&s->mtx);
}

static void sem_post_s(Semaphore *s)
{
	pthread_mutex_lock(&s->mtx);
	s->value++;
	pthread_cond_signal(&s->cnd);
	pthread_mutex_unlock(&s->mtx);
}

/* =========================================================
 * FIFO Buffer
 * ========================================================= */
#define MAX_BUF 128

static int even_buf[MAX_BUF];
static int odd_buf[MAX_BUF];
static int even_head = 0, even_tail = 0, count_even = 0;
static int odd_head  = 0, odd_tail  = 0, count_odd  = 0;
static int next_even_val = 0;  /* 0, 2, 4, ..., 48, 0, 2, ... */
static int next_odd_val  = 1;  /* 1, 3, 5, ..., 49, 1, 3, ... */

static int total(void) { return count_even + count_odd; }

/* Thread conditions */
static int can_a1(void) { return count_even < 10; }
static int can_a2(void) { return count_even > count_odd; }
static int can_b1(void) { return total() >= 3 && count_even > 0; }
static int can_b2(void) { return total() >= 7 && count_odd  > 0; }

/* =========================================================
 * Synchronization variables
 * ========================================================= */
static Semaphore mutex_sem;          /* mutual exclusion */
static Semaphore sem_a1, sem_a2;     /* condition semaphores for producers */
static Semaphore sem_b1, sem_b2;     /* condition semaphores for consumers */
static int wait_a1 = 0, wait_a2 = 0;
static int wait_b1 = 0, wait_b2 = 0;

/*
 * signal_waiters — called while holding mutex_sem.
 * For each thread type whose condition is now satisfied,
 * unblock ONE waiting thread of that type.
 */
static void signal_waiters(void)
{
	if (wait_a1 > 0 && can_a1()) { wait_a1--; sem_post_s(&sem_a1); }
	if (wait_a2 > 0 && can_a2()) { wait_a2--; sem_post_s(&sem_a2); }
	if (wait_b1 > 0 && can_b1()) { wait_b1--; sem_post_s(&sem_b1); }
	if (wait_b2 > 0 && can_b2()) { wait_b2--; sem_post_s(&sem_b2); }
}

/*
 * wait_for — helper: release mutex, block on cond semaphore,
 *            then re-acquire mutex.
 */
static void wait_for(Semaphore *cond, int *wait_count)
{
	(*wait_count)++;
	sem_post_s(&mutex_sem);     /* release lock */
	sem_wait_s(cond);           /* block until signaled */
	sem_wait_s(&mutex_sem);     /* re-acquire lock */
}

/* =========================================================
 * Thread functions
 * ========================================================= */

/* A1: produce even number if count_even < 10 */
static void *thread_a1(void *arg)
{
	int id = *(int *)arg;
	while (1) {
		sem_wait_s(&mutex_sem);

		/* Wait while condition is not met */
		while (!can_a1())
			wait_for(&sem_a1, &wait_a1);

		/* Produce */
		int val = next_even_val;
		next_even_val = (next_even_val + 2) % 50;
		even_buf[even_tail] = val;
		even_tail = (even_tail + 1) % MAX_BUF;
		count_even++;

		printf("[A1-%d] produced even=%2d  | even=%d odd=%d total=%d\n",
		       id, val, count_even, count_odd, total());

		signal_waiters();
		sem_post_s(&mutex_sem);

		usleep(80000);  /* 80 ms */
	}
	return NULL;
}

/* A2: produce odd number if count_even > count_odd */
static void *thread_a2(void *arg)
{
	int id = *(int *)arg;
	while (1) {
		sem_wait_s(&mutex_sem);

		while (!can_a2())
			wait_for(&sem_a2, &wait_a2);

		int val = next_odd_val;
		next_odd_val = (next_odd_val + 2) % 50;
		if (next_odd_val == 0) next_odd_val = 1;  /* keep it odd */
		odd_buf[odd_tail] = val;
		odd_tail = (odd_tail + 1) % MAX_BUF;
		count_odd++;

		printf("[A2-%d] produced odd =%2d  | even=%d odd=%d total=%d\n",
		       id, val, count_even, count_odd, total());

		signal_waiters();
		sem_post_s(&mutex_sem);

		usleep(80000);
	}
	return NULL;
}

/* B1: consume even number if total >= 3 */
static void *thread_b1(void *arg)
{
	int id = *(int *)arg;
	while (1) {
		sem_wait_s(&mutex_sem);

		while (!can_b1())
			wait_for(&sem_b1, &wait_b1);

		int val = even_buf[even_head];
		even_head = (even_head + 1) % MAX_BUF;
		count_even--;

		printf("[B1-%d] consumed even=%2d | even=%d odd=%d total=%d\n",
		       id, val, count_even, count_odd, total());

		signal_waiters();
		sem_post_s(&mutex_sem);

		usleep(120000);  /* 120 ms */
	}
	return NULL;
}

/* B2: consume odd number if total >= 7 */
static void *thread_b2(void *arg)
{
	int id = *(int *)arg;
	while (1) {
		sem_wait_s(&mutex_sem);

		while (!can_b2())
			wait_for(&sem_b2, &wait_b2);

		int val = odd_buf[odd_head];
		odd_head = (odd_head + 1) % MAX_BUF;
		count_odd--;

		printf("[B2-%d] consumed odd =%2d | even=%d odd=%d total=%d\n",
		       id, val, count_even, count_odd, total());

		signal_waiters();
		sem_post_s(&mutex_sem);

		usleep(120000);
	}
	return NULL;
}

/* =========================================================
 * Main
 * ========================================================= */
int main(void)
{
	printf("=== FIFO Buffer Sync: Semaphore Implementation ===\n");
	printf("Conditions:\n");
	printf("  A1: produce even if count_even < 10\n");
	printf("  A2: produce odd  if count_even > count_odd\n");
	printf("  B1: consume even if total >= 3\n");
	printf("  B2: consume odd  if total >= 7\n\n");

	/* Initialize semaphores */
	sem_init(&mutex_sem, 1);  /* binary: starts unlocked */
	sem_init(&sem_a1, 0);
	sem_init(&sem_a2, 0);
	sem_init(&sem_b1, 0);
	sem_init(&sem_b2, 0);

	/* Thread IDs */
	static int ids[8] = {1, 2, 1, 2, 1, 2, 1, 2};
	pthread_t threads[8];

	/* Create 2 of each thread type */
	pthread_create(&threads[0], NULL, thread_a1, &ids[0]);
	pthread_create(&threads[1], NULL, thread_a1, &ids[1]);
	pthread_create(&threads[2], NULL, thread_a2, &ids[2]);
	pthread_create(&threads[3], NULL, thread_a2, &ids[3]);
	pthread_create(&threads[4], NULL, thread_b1, &ids[4]);
	pthread_create(&threads[5], NULL, thread_b1, &ids[5]);
	pthread_create(&threads[6], NULL, thread_b2, &ids[6]);
	pthread_create(&threads[7], NULL, thread_b2, &ids[7]);

	/* Run for 10 seconds then exit */
	sleep(10);

	printf("\n=== Done. Final state: even=%d odd=%d total=%d ===\n",
	       count_even, count_odd, total());
	return 0;
}
