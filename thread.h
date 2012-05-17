#ifndef THREAD_DEFINED
#define THREAD_DEFINED

#include <iostream>
#include <cerrno>
#include <stdexcept>

#ifdef _WIN32
	#include <windows.h>
	#include "winsock2.h"
	#define FAMILY_WINDOWS
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__LINUX__) || defined(__linux__) || defined(MACOSX) || defined(__APPLE__) || defined	(__DARWIN__) || defined(__sun)
	#define FAMILY_UNIX
	#include <pthread.h>
	#include <semaphore.h>
#else
	#error NOT IMPLEMENTED
#endif

class IThread
{
protected:
	virtual void DoStart() = 0;
	virtual void DoStop() = 0;
	virtual void DoFinish() = 0;
	virtual bool DoRunning() = 0;

public:
	void Start() { return DoStart(); }
	void Stop() { return DoStop(); }
	void Finish() { return DoFinish(); }
	bool Running() { return DoRunning(); }
	virtual ~IThread() {}
} ;

template<class Class>
class CThread : public IThread
{
private:
	#ifdef FAMILY_UNIX
	pthread_t m_Thread;
	#else
	HANDLE m_Thread;
	#endif
	
	Class *m_pObject;
	void (Class::*m_pMethod)();
	bool m_Running;

	void Action()
	{
		(m_pObject->*m_pMethod)();
		m_Running = false;
	}

	static void *Run(void *pObject)
	{
		#ifdef FAMILY_UNIX
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
		#endif
		reinterpret_cast<CThread<Class> *>(pObject)->Action();
		return 0;
	}

protected:
	void DoStart()
	{
		if(m_Running)
			return;
		m_Running = true;
		#ifdef FAMILY_UNIX
		pthread_create(&m_Thread, NULL, &CThread::Run, this);
		#else
		m_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CThread::Run, this, 0, NULL);
		#endif
	}

	void DoStop()
	{
		if(!m_Running)
			return;
		#ifdef FAMILY_UNIX
		pthread_cancel(m_Thread);
		#else
		TerminateThread(m_Thread, 0);
		#endif
		m_Running = false;
	}

	void DoFinish()
	{
		if(!m_Running)
			return;
		#ifdef FAMILY_UNIX
		pthread_join(m_Thread, NULL);
		#else
		while(1)
		{
			if(m_Running == false)
				break;
		}
		return;
		#endif
	}

	bool DoRunning()
	{
		return m_Running;
	}

public:
	CThread(Class *pObject, void(Class::*pMethod)()) : m_pObject(pObject), m_pMethod(pMethod), m_Running(false) {}
} ;

class CMutex
{
private:
	#ifdef FAMILY_UNIX
	pthread_mutex_t m_Mutex;
	#else
	CRITICAL_SECTION m_Mutex;
	#endif
	bool m_Locked;
	
	
public:
	CMutex()
	{
		#ifdef FAMILY_UNIX
		pthread_mutex_init(&m_Mutex, NULL);
		#else
		InitializeCriticalSection(&m_Mutex);
		#endif
		m_Locked = false;
	}

	~CMutex()
	{
		#ifdef FAMILY_UNIX
		pthread_mutex_destroy(&m_Mutex);
		#else
		DeleteCriticalSection(&m_Mutex);
		#endif
	}

	int Lock()
	{
		int Ret;
		#ifdef FAMILY_UNIX
		Ret = pthread_mutex_lock(&m_Mutex);
		#else
		EnterCriticalSection(&m_Mutex);
		Ret = 0;
		#endif
		if(Ret == 0)
			m_Locked = true;
		return Ret;
	}

	int Unlock()
	{
		int Ret;
		#ifdef FAMILY_UNIX
		Ret = pthread_mutex_unlock(&m_Mutex);
		#else
		LeaveCriticalSection(&m_Mutex);
		Ret = 0;
		#endif
		if(Ret == 0)
			m_Locked = false;
		return Ret;
	}

	int TryLock()
	{
		int Ret;
		#ifdef FAMILY_UNIX
		Ret = pthread_mutex_trylock(&m_Mutex);
		#else
		Ret = TryEnterCriticalSection(&m_Mutex);
		Ret = Ret==0?1:0;
		#endif
		if(Ret == 0)
			m_Locked = true;
		return Ret;
	}

	bool IsLocked() { return m_Locked; }
} ;

class CSemaphore
{
private:
	const char *m_pName;
	#ifdef FAMILY_UNIX
	sem_t *m_pSemaphore;
	#else
	HANDLE m_Semaphore;
	#endif
public:
	CSemaphore(const char *pName, unsigned int Value, int Flags = O_EXCL | O_CREAT, int Mode = S_IWUSR | S_IRUSR): m_pName (pName)
	{
		#ifdef FAMILY_UNIX
		m_pSemaphore = sem_open(pName, Flags, Mode, Value);
		if(m_pSemaphore == SEM_FAILED)
			throw std::logic_error(strerror(errno));
		#else
		m_Semaphore = CreateSemaphore(
		NULL,	// pointer to security attributes
		Value,	// initial count
		Value,	// maximum count
		NULL	// pointer to semaphore-object name
		);
		#endif
	}
	~CSemaphore()
	{
		#ifdef FAMILY_UNIX
		sem_close(m_pSemaphore);
		sem_unlink(m_pName);
		#else
		#endif
	}
	int Wait()
	{
		#ifdef FAMILY_UNIX
		return sem_wait(m_pSemaphore);
		#else
		return WaitForSingleObject(
		m_Semaphore,	// handle to object to wait for
		INFINITE		// time-out interval in milliseconds
		);
		#endif
	}
	int TryWait()
	{
		#ifdef FAMILY_UNIX
		return sem_trywait(m_pSemaphore);
		#else
		return WaitForSingleObject(
		m_Semaphore,	// handle to object to wait for
		0				// time-out interval in milliseconds
		);
		#endif
	}
	int Post()
	{
		#ifdef FAMILY_UNIX
		return sem_post(m_pSemaphore);
		#else
		return ReleaseSemaphore(
		m_Semaphore,	// handle to the semaphore object
		1,				// amount to add to current count
		NULL			// address of previous count
		);
		return 0;
		#endif
	}
};

template<class Class>
CThread<Class>* CreateThread(Class *pObject, void(Class::*pMethod)()) { return new CThread<Class>(pObject, pMethod); }

#endif
