/*
 * main.cpp
 *
 *  Created on: 2016-8-4
 *      Author: root
 */

#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h> // sleep()

#include <iostream>
#include <string>
using namespace std;

#include "view_callback.h"

typedef bool (*example02_initfun)(view_callback*&);
typedef bool (*example02_destfun)();

typedef bool (*example_initfun)(view_callback*&);
typedef bool (*example_destfun)();
typedef int  (*example_repfun)(const char*, size_t);


view_callback* pViewCallback = 0;

void *example_handle = 0;
example_initfun example_init_func;
example_destfun example_dest_func;
example_repfun  example_rep_func;
bool load_example(){
	char *error;
	// open .so file.
	example_handle = dlopen("libexample.so",RTLD_LAZY);

	if(!example_handle)
	{
		cout << "open libexample.so file error, error:" <<
						dlerror() << endl;
		return false;
	}
	dlerror();

	example_init_func = (example_initfun)dlsym(example_handle, "init");
	example_dest_func = (example_destfun)dlsym(example_handle, "destory");
	example_rep_func  = (example_repfun)dlsym(example_handle, "response");


	if((error=dlerror())!=NULL)
	{
		cout << "open libexample.so file error, error:" << error << endl;
		return false;
	}

	return true;
}

void close_example(){
	if(example_handle){

		// Free resource.
		example_dest_func();

		dlclose(example_handle);
	}
}

void run_example(){
	// Set pViewCallback to user::p
	example_init_func(pViewCallback);
	cout << "run_example(), view_callback* p:" << pViewCallback << endl;

	cout << "run_example(), start to response" << endl;
	char a[4] = {0x01, 0x02, 0x03, 0x04};
	example_rep_func(&a[0], 4);
}




void *example02_handle = 0;
example02_initfun example02_init_func;
example02_destfun example02_dest_func;
bool load_example02(){

	char *error;
	// open .so file.
	example02_handle = dlopen("libexample02.so",RTLD_LAZY);

	if(!example02_handle)
	{
		cout << "open libexample02.so file error, error:" <<
						dlerror() << endl;
		return false;
	}
	dlerror();

	example02_init_func = (example02_initfun)dlsym(example02_handle, "init");
	example02_dest_func = (example02_destfun)dlsym(example02_handle, "destory");


	if((error=dlerror())!=NULL)
	{
		cout << "open libexample02.so file error, error:" << error << endl;
		return false;
	}

	return true;
}

void close_example02(){
	if(example02_handle){

		// Free resource.
		example02_dest_func();

		dlclose(example02_handle);
	}
}

void run_example02(){
	// Set pViewCallback from my.
	example02_init_func(pViewCallback);

	cout << "run_example02(), view_callback* p:" << pViewCallback << endl;
}


int main (int argc, char *argv[]) {

	load_example02();
	load_example();

	run_example02();
	run_example();

	sleep(4);
	close_example02();
	close_example();

	return 0;
}
