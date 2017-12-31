#include <iostream>
#include "ReferPtr.h"
#include <algorithm>
#include <memory>
using namespace std;

struct Test : public ReferredObject {
	~Test(){
		cout << "233" << endl;
	}
};

void tt(ReferPtr<Test> ptr){
	cout << ptr->GetReferCount() << endl;
}

int main(){
	
	auto ptr = ReferPtr<Test>::New();
	tt(ptr);
	return 0;
}