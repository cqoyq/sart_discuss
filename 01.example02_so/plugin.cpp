/*
 * plugin.cpp
 *
 *  Created on: 2016-8-4
 *      Author: root
 */

#include <iostream>
#include <string>
using namespace std;

#include "my.h"

my* my_obj = 0;

extern "C" {
	void init(view_callback*& p);
}

extern "C" {
	void destory();
}

void init(view_callback*& p){
	if(my_obj == 0){
		my_obj = new my();
		p = my_obj;
	}

	cout << "example02 message: call init(),my_obj:"<< my_obj << endl;
}

void destory(){
	cout << "example02 message: call destory(), my_obj:" << my_obj << endl;
	if(my_obj){
		delete my_obj;
		my_obj = 0;
	}
}

