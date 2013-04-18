/**
 * @file thread.h
 * @date Sep 13, 2012
 *
 * @brief
 * @version 1.0
 * @author Milos Subotic milos.subotic.sm@gmail.com
 *
 * @license GPLv3
 *
 */

#ifndef THREAD_H_
#define THREAD_H_

///////////////////////////////////////////////////////////////////////////////

// For std::runtime_error.
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////

typedef std::runtime_error concurent_error;

///////////////////////////////////////////////////////////////////////////////

#define __THROW_CONCURENT_ERROR() \
	do{ \
		throw concurent_error(std::string(__PRETTY_FUNCTION__) + " fails!"); \
	}while(0)

///////////////////////////////////////////////////////////////////////////////

#if defined(__ANDROID__) || defined(USE_PTHREAD_INSTEAD_STD_THREAD) || \
	!defined(_GLIBCXX_HAS_GTHREADS)

///////////////////////////////////////////////////////////////////////////////

extern "C" {
#include <pthread.h>
}

// For nanosleep().
#include <ctime>

///////////////////////////////////////////////////////////////////////////////

class thread{
protected:

	template<typename Functor>
	class ThreadArgs{
	public:
		ThreadArgs(Functor& callable_)
				: callable(callable_){
		}
		Functor callable;
	};

	template<typename Functor>
	static inline void* bodyForPthread(void* func){
		ThreadArgs<Functor>* threadArgs =
				reinterpret_cast<ThreadArgs<Functor>*>(func);
		threadArgs->callable();
		delete threadArgs;
		return NULL;
	}

public:
	template<typename Callable>
	thread(Callable& callable){
		ThreadArgs<Callable>* threadArgs = new ThreadArgs<Callable>(
				callable);

		int err = pthread_create(&_thread, NULL,
				bodyForPthread<Callable>,
				reinterpret_cast<void*>(threadArgs));
		if(err != 0){
			__THROW_CONCURENT_ERROR();
		}

	}

	~thread(){
	}

	void detach(){
		if(pthread_detach(_thread) != 0){
			__THROW_CONCURENT_ERROR();
		}
	}
	void join(){
		if(pthread_join(_thread, NULL) != 0){
			__THROW_CONCURENT_ERROR();
		}
	}

protected:
	pthread_t _thread;

};

class mutex{
	friend class condition_variable;
public:
	mutex(){
		if(pthread_mutex_init(&_mutex, NULL) != 0){
			__THROW_CONCURENT_ERROR();
		}
	}

	~mutex(){
		if(pthread_mutex_destroy(&_mutex) != 0){
			__THROW_CONCURENT_ERROR();
		}
	}

	void lock(){
		if(pthread_mutex_lock(&_mutex) != 0){
			__THROW_CONCURENT_ERROR();
		}
	}

	void unlock(){
		if(pthread_mutex_unlock(&_mutex) != 0){
			__THROW_CONCURENT_ERROR();
		}
	}
protected:
	pthread_mutex_t _mutex;
};

template<typename Mutex>
class unique_lock{
	friend class condition_variable;

public:
	unique_lock(Mutex& mutex){
		_mutex = &mutex;
		_mutex->lock();
	}

	~unique_lock(){
		_mutex->unlock();
	}

	void lock(){
		_mutex->lock();
	}

	void unlock(){
		_mutex->unlock();
	}

protected:
	Mutex* _mutex;

};

class condition_variable{
public:
	condition_variable(){
		if(pthread_cond_init(&_cond, NULL) != 0){
			__THROW_CONCURENT_ERROR();
		}
	}

	~condition_variable(){
		if(pthread_cond_destroy(&_cond) != 0){
			__THROW_CONCURENT_ERROR();
		}
	}

	void notify_one(){
		if(pthread_cond_signal(&_cond) != 0){
			__THROW_CONCURENT_ERROR();
		}
	}

	void notify_all(){
		if(pthread_cond_broadcast(&_cond) != 0){
			__THROW_CONCURENT_ERROR();
		}
	}

	void wait(unique_lock<mutex>& lock){
		if(pthread_cond_wait(&_cond, &lock._mutex->_mutex) != 0){
			__THROW_CONCURENT_ERROR();
		}
	}

protected:
	pthread_cond_t _cond;
};

static inline void sleepMs(uint ms){
	struct timespec req;
	req.tv_sec = ms / 1000;
	req.tv_nsec = (ms % 1000) * 1000000;

	if(nanosleep(&req, NULL) != 0){
		__THROW_CONCURENT_ERROR();
	}
}

#else

#ifndef _GLIBCXX_USE_NANOSLEEP
#define _GLIBCXX_USE_NANOSLEEP
#endif
#include <thread>
#include <mutex>

// If indexing in Eclipse do not work add __GXX_EXPERIMENTAL_CXX0X__ to
// Project -> Properties -> C/C++ General -> Paths and Symbols -> Symbols
// GNU C++ -> Add

using std::thread;
using std::mutex;
using std::unique_lock;
using std::condition_variable;

static inline void sleepMs(uint ms){
	std::chrono::milliseconds duration(ms);
	std::this_thread::sleep_for(duration);
}

#endif

#include <semaphore.h>

class semaphore{
private:
	sem_t _semapore;
public:

	enum Pshared {
		PROCESS_LOCAL = 0,
		INTERPROCESS_SHARED
	};

	semaphore(int initialValue = 0, Pshared pshared = PROCESS_LOCAL){
		if(sem_init(&_semapore, pshared, initialValue)){
			__THROW_CONCURENT_ERROR();
		}
	}

	~semaphore(){
		if(sem_destroy(&_semapore)){
			__THROW_CONCURENT_ERROR();
		}
	}

	void post(){
		if(sem_post(&_semapore)){
			__THROW_CONCURENT_ERROR();
		}
	}

	void wait(){
		if(sem_wait(&_semapore)){
			__THROW_CONCURENT_ERROR();
		}
	}

	semaphore& operator++(int){
		post();
		return *this;
	}

	semaphore& operator--(int){
		wait();
		return *this;
	}

	void truwait(){
		if(sem_trywait(&_semapore)){
			__THROW_CONCURENT_ERROR();
		}
	}

	int getValue(){
		int sval;
		// Always return 0.
		sem_getvalue(&_semapore, &sval);
		return sval;
	}


};

#if 0

#include <atomic>

class spin_mutex {
private:
	std::atomic<bool> lockVal;
public:
	spin_mutex()
			: lockVal(false){
	}

	void lock(){
		 while (lockVal.exchange(true)) { }

	}

	void unlock(){
		lockVal = false;
	}

	bool is_locked(){
		return lockVal.load();
	}
};

class spin_lock {
private:
	spin_mutex& _mutex;
public:
	spin_lock(const spin_lock&) = delete;
	const spin_lock& operator=(const spin_lock&) = delete;
public:
	spin_lock(spin_mutex & mutex)
			: _mutex(mutex){
		_mutex.lock();
	}

	~spin_lock(){
		_mutex.unlock();
	}
};

#endif

///////////////////////////////////////////////////////////////////////////////

#endif /* THREAD_H_ */
