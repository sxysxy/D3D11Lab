#include <iostream>
#include "ReferPtr.h"
#include <algorithm>
#include <memory>
using namespace std;
using namespace Utility;
struct Test1;

struct Test : public ReferredObject {
	ReferPtr<Test1> p;
	~Test(){
		cout << "233" << endl;
	}
	virtual void Release(){
		p.Release();
	}
};

struct Test1 : public ReferredObject {
	ReferPtr<Test> p;
	~Test1(){
		cout << "666" << endl;
	}
	virtual void Release(){
		p.Release();
	}
};

int main(){
	auto t = ReferPtr<Test>::New();
	t->p = ReferPtr<Test1>::New();
	t->p->p = t.Get();
	return 0;
}