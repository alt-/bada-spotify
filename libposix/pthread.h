/*
 Bada pthread
 Copyright (c) 2010 Markovtsev Vadim

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#ifndef PTHREAD_H_
#define PTHREAD_H_
//#include <sched.h>
#include <signal.h>
#include <stdbool.h>
//#include <setjmp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PTHREAD_STACK_SIZE Osp::Base::Runtime::DEFAULT_STACK_SIZE
#define PTHREAD_STACK_MINSIZE	16384
#define PTHREAD_THREADS_MAX	1024

#define MAX_SPIN_COUNT		50
#define SPIN_SLEEP_DURATION	2000001
#define PTHREAD_DESTRUCTOR_ITERATIONS 1

#define PTHREAD_KEYS_MAX	32

/* From sched.h */
struct sched_param {
  int sched_priority;
};
/* */

typedef struct _pthread_descr_struct*_pthread_descr;

#ifndef PTHREAD_INTERNAL
typedef void internal_lock;
typedef void internal_monitor;
typedef void* pthread_t;
#else
#include <FBaseRtThread.h>
typedef Osp::Base::Runtime::Thread* pthread_t;
typedef Osp::Base::Runtime::Mutex internal_lock;
typedef Osp::Base::Runtime::Monitor internal_monitor;
#endif

struct _pthread_fastlock { int __spinlock; };

#define PTHREAD_SPIN_LOCKED 1
#define PTHREAD_SPIN_UNLOCKED 0

/* Mutexes */
typedef struct {
//  Osp::Base::Runtime::Mutex *lock;
  internal_lock *lock;
  _pthread_descr owner;
  int kind;
  unsigned int count;
  bool acquired;
} pthread_mutex_t;

enum {
  PTHREAD_MUTEX_FAST_NP,
#define PTHREAD_MUTEX_FAST_NP PTHREAD_MUTEX_FAST_NP
  PTHREAD_MUTEX_RECURSIVE_NP,
#define PTHREAD_MUTEX_RECURSIVE_NP PTHREAD_MUTEX_RECURSIVE_NP
  PTHREAD_MUTEX_ERRORCHECK_NP,
#define PTHREAD_MUTEX_ERRORCHECK_NP PTHREAD_MUTEX_ERRORCHECK_NP
};

enum {
  PTHREAD_PROCESS_PRIVATE,
#define PTHREAD_PROCESS_PRIVATE PTHREAD_PROCESS_PRIVATE
  PTHREAD_PROCESS_SHARED
#define PTHREAD_PROCESS_SHARED PTHREAD_PROCESS_SHARED
};

#define PTHREAD_MUTEX_INITIALIZER \
{{PTHREAD_SPIN_UNLOCKED},0,PTHREAD_MUTEX_FAST_NP,0}

#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP \
{{PTHREAD_SPIN_UNLOCKED},0,PTHREAD_MUTEX_RECURSIVE_NP,0}

#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP \
{{PTHREAD_SPIN_UNLOCKED},0,PTHREAD_MUTEX_ERRORCHECK_NP,0}

typedef struct {
  int __mutexkind;
} pthread_mutexattr_t;

int pthread_mutexattr_init(pthread_mutexattr_t*attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t*attr);

//int pthread_mutexattr_getkind_np(const pthread_mutexattr_t*attr,int*kind);
//int pthread_mutexattr_setkind_np(pthread_mutexattr_t*attr,int kind);

int pthread_mutex_init(pthread_mutex_t*mutex,
		const pthread_mutexattr_t*mutexattr);
int pthread_mutex_lock(pthread_mutex_t*mutex);
int pthread_mutex_unlock(pthread_mutex_t*mutex);
int pthread_mutex_trylock(pthread_mutex_t*mutex);
int pthread_mutex_destroy(pthread_mutex_t*mutex);

/* Conditions */
typedef void* pthread_condattr_t;

typedef struct {
//  Osp::Base::Runtime::Monitor *lock;
  internal_monitor *lock;
  _pthread_descr wait_chain;
} pthread_cond_t;

#define PTHREAD_COND_INITIALIZER \
{{PTHREAD_SPIN_UNLOCKED},0}

int pthread_cond_init(pthread_cond_t*cond,pthread_condattr_t*cond_attr);
int pthread_cond_destroy(pthread_cond_t*cond);
int pthread_cond_signal(pthread_cond_t*cond);
int pthread_cond_broadcast(pthread_cond_t*cond);
int pthread_cond_timedwait(pthread_cond_t*cond,pthread_mutex_t*mutex,
			   const struct timespec*abstime);
int pthread_cond_wait(pthread_cond_t*cond,pthread_mutex_t*mutex);

// These two functions are not described in the standard. Use them as follows:
// [thread #1]
//   pthread_cond_enter(cond);
//   pthread_cond_wait(cond, null);
//   pthread_cond_exit(cond);
//
// [thread #2]
//   pthread_cond_enter(cond);
//   pthread_cond_broadcast(cond);
//   pthread_cond_exit(cond);
//
// The mutex style locking will result in a dead lock.
int pthread_cond_enter(pthread_cond_t*cond);
int pthread_cond_exit(pthread_cond_t*cond);

/* only for completeness (always return NULL) */
int pthread_condattr_init(pthread_condattr_t*attr);
int pthread_condattr_destroy(pthread_condattr_t*attr);
int pthread_condattr_getpshared(const pthread_condattr_t*attr,int*pshared);
int pthread_condattr_setpshared(pthread_condattr_t*attr,int pshared);

/* thread specific variables */
typedef unsigned int pthread_key_t;
/*
int pthread_key_create(pthread_key_t*key,void(*destructor)(const void*));
int pthread_key_delete(pthread_key_t key);
int pthread_setspecific(pthread_key_t key,const void*value);
const void*pthread_getspecific(pthread_key_t key);
*/

/* Attributes for threads.  */
typedef struct {
  int		__detachstate;
  int		__schedpolicy;
  struct sched_param	__schedparam;
  int		__inheritsched;
  int		__scope;
  void *	__stackaddr;
  unsigned long __stacksize;
} pthread_attr_t;

enum {
  PTHREAD_CREATE_JOINABLE,
#define PTHREAD_CREATE_JOINABLE PTHREAD_CREATE_JOINABLE
  PTHREAD_CREATE_DETACHED
#define PTHREAD_CREATE_DETACHED PTHREAD_CREATE_DETACHED
};

enum {
  PTHREAD_EXPLICIT_SCHED,
#define PTHREAD_EXPLICIT_SCHED PTHREAD_EXPLICIT_SCHED
  PTHREAD_INHERIT_SCHED
#define PTHREAD_INHERIT_SCHED PTHREAD_INHERIT_SCHED
};

enum {	/* for completeness */
  PTHREAD_SCOPE_SYSTEM,
#define PTHREAD_SCOPE_SYSTEM PTHREAD_SCOPE_SYSTEM
  PTHREAD_SCOPE_PROCESS
#define PTHREAD_SCOPE_PROCESS PTHREAD_SCOPE_PROCESS
};

int pthread_attr_init(pthread_attr_t*attr);
int pthread_attr_destroy(pthread_attr_t*attr);

int pthread_attr_setdetachstate(pthread_attr_t*attr,const int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t*attr,int*detachstate);
/*
int pthread_attr_setschedpolicy(pthread_attr_t*attr,const int policy);
int pthread_attr_getschedpolicy(const pthread_attr_t*attr,int*policy);

int pthread_attr_setschedparam(pthread_attr_t*attr,
				const struct sched_param*param);
int pthread_attr_getschedparam(const pthread_attr_t*attr,
				struct sched_param*param);

int pthread_attr_setinheritsched(pthread_attr_t*attr,const int inherit);
int pthread_attr_getinheritsched(const pthread_attr_t*attr,int*inherit);

int pthread_attr_setscope(pthread_attr_t*attr,const int scope);
int pthread_attr_getscope(const pthread_attr_t*attr,int*scope);

int pthread_attr_setstackaddr(pthread_attr_t*attr,void*stack);
int pthread_attr_getstackaddr(const pthread_attr_t*attr,void**stack);

int pthread_attr_setstacksize(pthread_attr_t*attr,const size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t*attr,size_t*stacksize);

int pthread_setschedparam(const pthread_t target_thread,const int policy,
			  const struct sched_param*param);
int pthread_getschedparam(const pthread_t target_thread,int*policy,
			  struct sched_param*param);
*/
/* ONCE */
typedef int pthread_once_t;
#define PTHREAD_ONCE_INIT	PTHREAD_SPIN_UNLOCKED

//int pthread_once(pthread_once_t*once_control,void(*init_routine)(void));

/* CANCEL */

enum {
  PTHREAD_CANCEL_ENABLE,
#define PTHREAD_CANCEL_ENABLE PTHREAD_CANCEL_ENABLE
  PTHREAD_CANCEL_DISABLE,
#define PTHREAD_CANCEL_DISABLE PTHREAD_CANCEL_DISABLE
};

enum {
  PTHREAD_CANCEL_DEFERRED,
#define PTHREAD_CANCEL_DEFERRED PTHREAD_CANCEL_DEFERRED
  PTHREAD_CANCEL_ASYNCHRONOUS,
#define PTHREAD_CANCEL_ASYNCHRONOUS PTHREAD_CANCEL_ASYNCHRONOUS
};

#define PTHREAD_CANCELED ((void *) -1)

int pthread_kill(pthread_t thread,int sig);
int pthread_cancel(pthread_t thread);
/*int pthread_setcancelstate(int state,int*oldstate);

int pthread_setcanceltype(int type,int*oldtype);

void pthread_testcancel(void);
*/
/* CLEANUP */
/*
void pthread_cleanup_push(void(*routine)(void*),void*arg);
void pthread_cleanup_pop(int execute);

void pthread_cleanup_push_defer_np(void(*routine)(void*),void*arg);
void pthread_cleanup_pop_restore_np(int execute);
*/
/* FORK */
/*
pid_t pthread_atfork(void(*prepare)(void),void(*parent)(void),
		     void(*child)(void));
*/
/* THREADS */
pthread_t pthread_self(void);

int pthread_create(pthread_t*__threadarg,
		const pthread_attr_t*__attr,
		void*(*__start_routine)(void *),
		void*__arg);

void pthread_exit(void*__retval) __attribute__((__noreturn__));

int pthread_join(pthread_t __th,void**__thread_return);

int pthread_detach(pthread_t __th);
int pthread_equal(pthread_t __thread1,pthread_t __thread2);

//int pthread_sigmask(int how,const sigset_t*newset,sigset_t*oldset);

#ifdef __cplusplus
}
#endif

#endif /* PTHREAD_H_ */
