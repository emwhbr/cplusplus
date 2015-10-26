#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "signal_support.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////
#define SIG_TEST_HANDLER   SIGUSR1

#define SIG_TEST_WAIT1  (SIGRTMIN+10)
#define SIG_TEST_WAIT2  (SIGRTMIN+11)

/////////////////////////////////////////////////////////////////////////////
//               Function prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Global variables
/////////////////////////////////////////////////////////////////////////////
SIGNAL_SET g_signal_set;

/////////////////////////////////////////////////////////////////////////////
//               Private functions
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

static void test_signal_handler(int sig)
{
  printf("SIGNAL HANDLER : Got signal(%d)\n", sig);
}

////////////////////////////////////////////////////////////////

static void *signal_waiter(void *arg)
{
  long rc;
  int the_sig;

  // JOE: Make GCC happy
  if (arg) {;}

  // Wait signal 1/2
  printf("THREAD: wait for signal\n");
  rc = sig_sup_wait_signal(g_signal_set, true, the_sig);
  if (rc != SIGNAL_SUPPORT_SUCCESS) {
    printf("ERROR(THREAD) : wait signal, rc = %ld\n", rc);
  }
  printf("THREAD : got signal(%d)\n", the_sig);

  // wait signal 2/2
  the_sig = -1;
  printf("THREAD: wait for signal(timed)\n");
  rc = sig_sup_wait_signal_timed(g_signal_set, 5.34, the_sig);
  if (rc != SIGNAL_SUPPORT_SUCCESS) {
    printf("ERROR(THREAD) : wait signal(timed), rc = %ld\n", rc);
  }
  printf("THREAD : got signal(%d)\n", the_sig);

  // Main will never send this signal, so this should fail
  the_sig = -1;
  printf("THREAD: wait for signal(peek)\n");
  rc = sig_sup_wait_signal(g_signal_set, false, the_sig);
  if (rc != SIGNAL_SUPPORT_SUCCESS) {
    printf("ERROR(THREAD) : wait signal(peek), rc = %ld\n", rc);
  }
  printf("THREAD : got signal(%d)\n", the_sig);

  printf("THREAD : finished\n");
  return NULL;
}

/////////////////////////////////////////////////////////////////////////////
//               Main application
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  long rc;
  int status;
  pthread_t signal_thread_id;

  // JOE: Make GCC happy
  if (argc) {;}
  if (argv) {;}

  // Mask signals to wait for.
  // All threads inherit the signal mask from their creator.
  g_signal_set.insert(SIG_TEST_WAIT1);
  g_signal_set.insert(SIG_TEST_WAIT2);

  rc = sig_sup_mask(g_signal_set);
  if (rc != SIGNAL_SUPPORT_SUCCESS) {
    printf("ERROR : mask signals, rc = %ld\n", rc);
  }

  // Create the sigwait thread.
  printf("MAIN : create sigwaiter thread\n");
  status = pthread_create(&signal_thread_id,
			  NULL,
			  signal_waiter,
			  NULL);
  if (status != 0) {
    printf("ERROR : create sigwaiter thread\n");
  }

  // Install signal handler for test signal
  rc = sig_sup_define_signal_handler(SIG_TEST_HANDLER,
				     test_signal_handler);
  if (rc != SIGNAL_SUPPORT_SUCCESS) {
    printf("ERROR : install handler, rc = %ld\n", rc);
  }

  // Send signal that wakes handler
  printf("MAIN : send signal to self\n");
  rc = sig_sup_send_signal_self(SIG_TEST_HANDLER);
  if (rc != SIGNAL_SUPPORT_SUCCESS) {
    printf("ERROR : send signal, rc = %ld\n", rc);
  }
  sleep(1);

  // Send signal that wakes thread 1/2
  printf("MAIN : send signal to thread\n");
  rc = sig_sup_send_signal(SIG_TEST_WAIT1, signal_thread_id);
  if (rc != SIGNAL_SUPPORT_SUCCESS) {
    printf("ERROR : send signal, rc = %ld\n", rc);
  }

  // Send signal that wakes thread 2/2
  sleep(5); // Thread is waiting for signal using 5.34s timeout
  printf("MAIN : send signal to thread\n");
  rc = sig_sup_send_signal(SIG_TEST_WAIT2, signal_thread_id);
  if (rc != SIGNAL_SUPPORT_SUCCESS) {
    printf("ERROR : send signal, rc = %ld\n", rc);
  }
  sleep(2);

  printf("MAIN : finished\n");
  exit(EXIT_SUCCESS);
}
