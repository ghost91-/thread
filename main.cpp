#include "thread.h"
#include <iostream>
#include <stdexcept>

class CClass1
{
	CSemaphore *m_pSemaphore;
	public:
	CClass1(CSemaphore *pSemaphore) : m_pSemaphore(pSemaphore) {}

	void Function()
	{
		for(int i = 0; i < 5; i++)
		{
			m_pSemaphore->Wait();
			std::cout<<"Thread 1 has decremented the Semaphore"<<std::endl;
			m_pSemaphore->Post();
		}
		return;
	}
} ;

class CClass2
{
	CSemaphore *m_pSemaphore;
	public:
	CClass2(CSemaphore *pSemaphore) : m_pSemaphore(pSemaphore) {}

	void Function()
	{
		for(int i = 0; i < 5; i++)
		{
			m_pSemaphore->Wait();
			std::cout<<"Thread 2 has decremented the Semaphore"<<std::endl;
			m_pSemaphore->Post();
		}
		return;
	}
} ;

int main()
{
	try
	{
		CSemaphore Semaphore("/semaphore.sem", 1);
		CClass1 Class1(&Semaphore);
		CClass2 Class2(&Semaphore);
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
		std::cout<<"Semaphore error: "<<except.what()<<std::endl;
		return 1;
	}

	return 0;
}
