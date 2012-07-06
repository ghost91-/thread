#include "thread.h"
#include <iostream>
#ifdef _WIN32
	#include <windows.h>
	#define sleep(t) Sleep(t*1000)
#endif

class CClass1
{
	CMutex *m_pMutex;
	public:
	CClass1(CMutex *pMutex) : m_pMutex(pMutex) {}

	void Function()
	{
		for(int i = 0; i < 5; i++)
		{
			m_pMutex->Lock();
			std::cout<<"Thread 1 has locked the Mutex"<<std::endl;
			sleep(1);
			m_pMutex->Unlock();
		}
		return;
	}
} ;

class CClass2
{
	CMutex *m_pMutex;
	public:
	CClass2(CMutex *pMutex) : m_pMutex(pMutex) {}

	void Function()
	{
		for(int i = 0; i < 5; i++)
		{
			m_pMutex->Lock();
			std::cout<<"Thread 2 has locked the Mutex"<<std::endl;
			sleep(1);
			m_pMutex->Unlock();
		}
		return;
	}
} ;

int main()
{
	try
	{
		CMutex Mutex;
		CClass1 Class1(&Mutex);
		CClass2 Class2(&Mutex);
		IThread *pThread1 = CreateThread(&Class1, &CClass1::Function);
		IThread *pThread2 = CreateThread(&Class2, &CClass2::Function);
		pThread1->Start();
		pThread2->Start();
		pThread1->Finish();
		pThread2->Finish();
		delete pThread1;
		delete pThread2;
	}
	catch(std::logic_error &except)
	{
		std::cout<<"Mutex error: "<<except.what()<<std::endl;
		return 1;
	}

	return 0;
}
