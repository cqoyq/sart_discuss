/*
 * main.cpp
 *
 *  Created on: 2016-8-5
 *      Author: root
 */

#include "server.h"

int main (int argc, char *argv[]) {

	my_mgr::construct();	 			// Create tcp_handler_manager.

	my_task::construct(
				top					// thread_task queue top callback
				);


	try{
		 boost::asio::io_service io_service;
		 string ipv4 = "127.0.0.1";
		 u16_t port = 18000;
		 my_server server(io_service, ipv4, port);

		 io_service.run();

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	my_mgr::release();

	my_task::release();

	return 0;
}


