/*
 * sync_monitor/main.c
 *
 * FIFO Buffer Synchronization — Monitor Implementation
 *
 * Problem:
 *   A1: produce consecutive even numbers (mod 50) if count_even < 10
 *   A2: produce consecutive odd  numbers (mod 50) if count_even > count_odd
 *   B1: consume even numbers  if total >= 3
 *   B2: consume odd  numbers  if total >= 7
 *
 * Approach:
 *   A Monitor encapsulates the buffer and ALL synchronization.
 *   - One pthread_mutex_t  → "monitor lock" (only one thread inside at a time)
 *   - Four pthread_cond_t  → one per thread type (A1, A2, B1, B2)
 *
 *   Each operation is a monitor method:
 *     1. Lock the monitor mutex
 *     2. Wait (pthread_cond_wait) while the condition is not met
 *        (wait automatically releases the lock and re-acquires on wake)
 *     3. Perform the operation
 *     4. Broadcast to ALL condition variables so every waiting thread
 *        re-evaluates its condition
 *     5. Unlock the monitor mutex
 *
 *   Key difference from semaphore version:
 *     - No manual wait counters needed
 *     - pthread_cond_broadcast wakes ALL waiters; each re-checks its own condition
 *     - Monitor invariant: buffer state is always consistent inside the monitor
 *
 * Author: SyafiraRosa — 348296
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/* =========================================================
 * Monitor definition
 * ========================================================= */
#define MAX_BUF 128

typedef struct {
	/* FIFO queues */
	int even_buf[MAX_BUF];
	int odd_buf[MAX_BUF];
	int even_head, even_tail, count_even;
	int odd_head,  odd_tail,  count_odd;

	/* Sequence generators */
	int next_even;   /* 0, 2, 4, ..., 48, 0, ... */
	int next_odd;    /* 1, 3, 5, ..., 49, 1, ... */

	/* Monitor synchronization */
	pthread_mutex_t lock;
	pthread_cond_t  cond_a1;   /* A1 waits here */
	pthread_cond_t  cond_a2;   /* A2 waits here */
	pthread_cond_t  cond_b1;   /* B1 waits here */
	pthread_cond_t  cond_b2;   /* B2 waits here */
} Monitor;

/* Initialize the monitor */
static void monitor_init(Monitor *m)
{
	m->even_head = m->even_tail = m->count_even = 0;
	m->odd_head  = m->odd_tail  = m->count_odd  = 0;
	m->next_even = 0;
	m->next_odd  = 1;

	pthread_mutex_init(&m->lock, NULL);
	pthread_cond_init(&m->cond_a1, NULL);
	pthread_cond_init(&m->cond_a2, NULL);
	pthread_cond_init(&m->cond_b1, NULL);
	pthread_cond_init(&m->cond_b2, NULL);
}

/* Helper: total items in buffer */
static int mon_total(Monitor *m)
{
	return m->count_even + m->count_odd;
}

/*
 * notify_all — wake ALL waiting threads so they re-check their conditions.
 * Called after every state change while the lock is held.
 */
static void notify_all(Monitor *m)
{
	pthread_cond_broadcast(&m->cond_a1);
	pthread_cond_broadcast(&m->cond_a2);
	pthread_cond_broadcast(&m->cond_b1);
	pthread_cond_broadcast(&m->cond_b2);
}

/* =========================================================
 * Monitor methods (each is an atomic operation)
 * ========================================================= */

/* A1 monitor method: produce an even number */
static int monitor_produce_even(Monitor *m, int thread_id)
{
	pthread_mutex_lock(&m->lock);

	/* Wait while condition not satisfied (always loop — spurious wakeups) */
	while (m->count_even >= 10)
		pthread_cond_wait(&m->cond_a1, &m->lock);

	/* Produce */
	int val = m->next_even;
	m->next_even = (m->next_even + 2) % 50;
	m->even_buf[m->even_tail] = val;
	m->even_tail = (m->even_tail + 1) % MAX_BUF;
	m->count_even++;

	printf("[A1-%d] produced even=%2d  | even=%d odd=%d total=%d\n",
	       thread_id, val, m->count_even, m->count_odd, mon_total(m));

	notify_all(m);  /* notify others that state changed */
	pthread_mutex_unlock(&m->lock);
	return val;
}

/* A2 monitor method: produce an odd number */
static int monitor_produce_odd(Monitor *m, int thread_id)
{
	pthread_mutex_lock(&m->lock);

	while (m->count_even <= m->count_odd)
		pthread_cond_wait(&m->cond_a2, &m->lock);

	int val = m->next_odd;
	m->next_odd = (m->next_odd + 2) % 50;
	if (m->next_odd == 0) m->next_odd = 1;  /* keep odd */
	m->odd_buf[m->odd_tail] = val;
	m->odd_tail = (m->odd_tail + 1) % MAX_BUF;
	m->count_odd++;

	printf("[A2-%d] produced odd =%2d  | even=%d odd=%d total=%d\n",
	       thread_id, val, m->count_even, m->count_odd, mon_total(m));

	notify_all(m);
	pthread_mutex_unlock(&m->lock);
	return val;
}

/* B1 monitor method: consume an even number */
static int monitor_consume_even(Monitor *m, int thread_id)
{
	pthread_mutex_lock(&m->lock);

	while (mon_total(m) < 3 || m->count_even == 0)
		pthread_cond_wait(&m->cond_b1, &m->lock);

	int val = m->even_buf[m->even_head];
	m->even_head = (m->even_head + 1) % MAX_BUF;
	m->count_even--;

	printf("[B1-%d] consumed even=%2d | even=%d odd=%d total=%d\n",
	       thread_id, val, m->count_even, m->count_odd, mon_total(m));

	notify_all(m);
	pthread_mutex_unlock(&m->lock);
	return val;
}

/* B2 monitor method: consume an odd number */
static int monitor_consume_odd(Monitor *m, int thread_id)
{
	pthread_mutex_lock(&m->lock);

	while (mon_total(m) < 7 || m->count_odd == 0)
		pthread_cond_wait(&m->cond_b2, &m->lock);

	int val = m->odd_buf[m->odd_head];
	m->odd_head = (m->odd_head + 1) % MAX_BUF;
	m->count_odd--;

	printf("[B2-%d] consumed odd =%2d | even=%d odd=%d total=%d\n",
	       thread_id, val, m->count_even, m->count_odd, mon_total(m));

	notify_all(m);
	pthread_mutex_unlock(&m->lock);
	return val;
}

/* =========================================================
 * Thread argument structure
 * ========================================================= */
typedef struct {
	Monitor *mon;
	int id;
} ThreadArg;

/* =========================================================
 * Thread functions — just call the monitor method in a loop
 * ========================================================= */
static void *thread_a1(void *arg)
{
	ThreadArg *a = (ThreadArg *)arg;
	while (1) {
		monitor_produce_even(a->mon, a->id);
		usleep(80000);
	}
	return NULL;
}

static void *thread_a2(void *arg)
{
	ThreadArg *a = (ThreadArg *)arg;
	while (1) {
		monitor_produce_odd(a->mon, a->id);
		usleep(80000);
	}
	return NULL;
}

static void *thread_b1(void *arg)
{
	ThreadArg *a = (ThreadArg *)arg;
	while (1) {
		monitor_consume_even(a->mon, a->id);
		usleep(120000);
	}
	return NULL;
}

static void *thread_b2(void *arg)
{
	ThreadArg *a = (ThreadArg *)arg;
	while (1) {
		monitor_consume_odd(a->mon, a->id);
		usleep(120000);
	}
	return NULL;
}

/* =========================================================
 * Main
 * ========================================================= */
static Monitor mon;

int main(void)
{
	printf("=== FIFO Buffer Sync: Monitor Implementation ===\n");
	printf("Conditions:\n");
	printf("  A1: produce even if count_even < 10\n");
	printf("  A2: produce odd  if count_even > count_odd\n");
	printf("  B1: consume even if total >= 3\n");
	printf("  B2: consume odd  if total >= 7\n\n");

	monitor_init(&mon);

	/* Thread arguments */
	ThreadArg args[8];
	pthread_t threads[8];
	int i;

	for (i = 0; i < 8; i++) {
		args[i].mon = &mon;
		args[i].id  = (i % 2) + 1;  /* IDs: 1 or 2 */
	}

	/* Create 2 of each thread type */
	pthread_create(&threads[0], NULL, thread_a1, &args[0]);
	pthread_create(&threads[1], NULL, thread_a1, &args[1]);
	pthread_create(&threads[2], NULL, thread_a2, &args[2]);
	pthread_create(&threads[3], NULL, thread_a2, &args[3]);
	pthread_create(&threads[4], NULL, thread_b1, &args[4]);
	pthread_create(&threads[5], NULL, thread_b1, &args[5]);
	pthread_create(&threads[6], NULL, thread_b2, &args[6]);
	pthread_create(&threads[7], NULL, thread_b2, &args[7]);

	/* Run for 10 seconds */
	sleep(10);

	printf("\n=== Done. Final state: even=%d odd=%d total=%d ===\n",
	       mon.count_even, mon.count_odd, mon.count_even + mon.count_odd);
	return 0;
}
