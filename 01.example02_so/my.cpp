/*
 * my.cpp
 *
 *  Created on: 2016-8-4
 *      Author: root
 */

#include "my.h"

#include "../01.example_so/atest.h"

#include <iostream>
#include <string>
using namespace std;

my::my() {
	// TODO Auto-generated constructor stub

}

my::~my() {
	// TODO Auto-generated destructor stub
}


void my::call(void*& data){
	atest* p = static_cast<atest*>(data);
	if(p)
		cout << "example02 message: call my::call(), my_obj:" << this << ",data.m:" << p->m << endl;
}
