#include "thread.h"
#include <iostream>

class CClass1
{
	public:
	void Function()
	{
		std::cout << "Test1\n";
		return;
	}
} ;

class CClass2
{
	public:
	void Function()
	{
		std::cout << "Test2\n";
		return;
	}
} ;

int main()
{
	CClass1 Class1;
	CClass2 Class2;
	IThread *pThread1 = CreateThread(&Class1, &CClass1::Function);
	IThread *pThread2 = CreateThread(&Class2, &CClass2::Function);
	pThread1->Start();
	pThread2->Start();
	pThread1->Finish();
	pThread2->Finish();
	delete pThread1;
	delete pThread2;
	return 0;
}
