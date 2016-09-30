#include <iostream>

#include "asiohandler.h"

#include "bingo/string.h"

AMQP::Channel* p_channl;

int amount = 0;

int main(void)
{
	while(true){
		boost::asio::io_service ioService;
		AsioHandler handler(ioService);
		handler.connect("127.0.0.1", 5672);

		AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");
	//    connection.setHeartbeat(10);

		cout << "heartjump:" << connection.heartbeat() << endl;

		AMQP::Channel channel(&connection);
		channel.declareQueue("hello");
		p_channl = &channel;


		channel.onError([](const char *message) {

			// report error
			std::cout << "channel error: " << message << std::endl;
		});

	//    channel.consume("hello", AMQP::noack).onReceived(
	//            [](const AMQP::Message &message,
	//                       uint64_t deliveryTag,
	//                       bool redelivered)
	//            {
	////                std::cout <<" [x] Received "<<message.message() << std::endl;
	//            });

		// callback function that is called when the consume operation starts
		auto startCb = [](const std::string &consumertag) {

			std::cout << "consume operation started," << consumertag << std::endl;
		};

		// callback function that is called when the consume operation failed
		auto errorCb = [](const char *message) {

			std::cout << "consume operation failed" << std::endl;
		};

		// callback operation when a message was received
		auto messageCb = [](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {

			amount++;

	//        std::cout << "message received:" << message.message() << std::endl;
			bingo::string_ex t;
			std::cout << "msg size:" << message.message() .length() <<
					",data:" << t.stream_to_string(message.message().c_str(), message.message() .length()  ) << ",amount:" << amount << endl;

	//        char heart[8] = {0x08, 0x00, 0x00, 0x00,
	//                            								0x00, 0x00, 0x00, 0xce};
	//
	//        bool re = p_channl->publish("", "hello", &heart[0], 8);

			// acknowledge the message
	//        channel.ack(deliveryTag);
		};


		channel.consume("", AMQP::noack)
			.onReceived(messageCb)
			.onSuccess(startCb)
			.onError(errorCb);

		std::cout << " [*] Waiting for messages. To exit press CTRL-C\n";

		ioService.run();

		cout << "ios exit!" << endl;
	}
    return 0;
}
