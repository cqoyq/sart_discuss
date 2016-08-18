/*
 * main.cpp

 *
 *  Created on: 2016-8-5
 *      Author: root
 */

#include <fstream>
#include <iostream>
using namespace std;

#include "package.h"
#include "client.h"


#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#define EVERY_READ_DATA_LENGTH 1024

void read_file_size(){
	path p("/opt/src/example.tar.gz");
	size_t size = file_size(p);

	// Set next value.
	inext += size / EVERY_READ_DATA_LENGTH;
	if(size % EVERY_READ_DATA_LENGTH != 0 )
		inext++;
}

void read_file(){

	this_thread::sleep(seconds(4));

	read_file_size();

	ifstream fin("/opt/src/example.tar.gz",ios::binary);
	if(!fin){
		cout<<"File open error!\n" << endl;
		return;
	}

	char c[EVERY_READ_DATA_LENGTH];
	while(!fin.eof())
	{
		fin.read(c,EVERY_READ_DATA_LENGTH);
		size_t data_size = fin.gcount();

		// pack data.
		char* out = 0;
		size_t out_size = 0;
		pack(c, data_size, out, out_size);

		if(data_size > 0){
			cout << "out_size:" << out_size << ",next:" << inext << ",next_num:" << inext_num << endl;

			run_client(out, out_size);


			this_thread::sleep(microseconds(1));

//			if(inext_num == 10000)
//				break;
		}

		inext_num++;
	}

	fin.close();
}

int main (int argc, char *argv[]) {

	my_mgr::construct();

	thread t(read_file);

	try{
		 boost::asio::io_service io_service;
		 string ipv4 = "127.0.0.1";
		 u16_t port = 18000;
		 my_client c(io_service, ipv4, port);

		 io_service.run();

		 t.join();

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	my_mgr::release();

	return 0;
}

