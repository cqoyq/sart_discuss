#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/thread.hpp>
using namespace boost;

#include "asiohandler.h"

int num = 0;
boost::asio::io_service ioService;

AsioHandler* handler;
AMQP::Connection* connection;
AMQP::Channel* channel;

void next(){
	channel = new AMQP::Channel(connection);

	channel->onError([&](const char *message) {

		// report error
		std::cout << "channel error: " << message << std::endl;
	});

	channel->onReady([&]()
	{
		if(handler->connected())
		{
//				char d[1104] = {0x00};
//				memset(d, 0x00, 1104);
//				d[0] = 0x01;
//				d[1] = 0x02;
//				d[2] = 0x03;
//				d[3] = 0x04;
//				d[1102] = 0xFE;
//				d[1103] = 0xFF;
//				bool re = channel->publish("", "hello", &d[0], 1104);
				bool re = channel->publish("", "hello", "Hello World!");
				std::cout << " [x] Sent 'Hello World!'" << ",return:" << re << std::endl;


				channel->close();
				delete channel;

				boost::this_thread::sleep(boost::posix_time::milliseconds(500));

//				return;

				if(num == 400)
					return;
				else{
					num++;
					next();
				}

		}
	});
}

void run(){

	handler = new AsioHandler(ioService);
	handler->connect("127.0.0.1", 5672);


	connection = new AMQP::Connection(handler, AMQP::Login("guest", "guest"), "/");

	next();

	ioService.run();
};

int main (int argc, char *argv[])
{
	boost::thread t1(run);
	t1.join();


    return 0;
}
