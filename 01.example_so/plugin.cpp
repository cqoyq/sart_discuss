/*
 * plugin.cpp
 *
 *  Created on: 2016-8-4
 *      Author: root
 */

#include <iostream>
#include <string>
using namespace std;

#include "atest.h"
#include "user.h"

extern "C" {
	void init(view_callback*& p);
}

extern "C" {
	void destory();
}

extern "C" {
	int response(const char*, size_t);
}

user* user_obj = 0;

void init(view_callback*& p){
	if(user_obj == 0)
		user_obj = new user();

	user_obj->p = p;
	cout << "example message: call init(),user_obj:"<< user_obj
			<< ",p:" << p << endl;
}

void destory(){
	cout << "example message: call destory(), user_obj:" << user_obj << endl;
	if(user_obj){
		delete user_obj;
		user_obj = 0;
	}
}

int response(const char* data, size_t data_size){

	cout << "example message: call response(), user_obj:" << user_obj << endl;
	atest* p = new atest();
	void* p1 = static_cast<void*>(p);
	user_obj->p->call(p1);
	delete p;

	return 0;
};

