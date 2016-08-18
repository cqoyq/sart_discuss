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
				bool re = channel->publish("", "hello", "Hello World!");
				std::cout << " [x] Sent 'Hello World!'" << ",return:" << re << std::endl;


				channel->close();
				delete channel;

				boost::this_thread::sleep(boost::posix_time::seconds(2));

				if(num == 4)
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
	handler->connect("192.168.2.80", 5672);


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
