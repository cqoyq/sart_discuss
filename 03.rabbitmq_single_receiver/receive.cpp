#include <iostream>

#include "asiohandler.h"

#include "bingo/string.h"

int main(void)
{
    boost::asio::io_service ioService;
    AsioHandler handler(ioService);
    handler.connect("192.168.2.80", 5672);

    AMQP::Connection connection(&handler, AMQP::Login("guest", "guest"), "/");

    AMQP::Channel channel(&connection);
    channel.declareQueue("hello");


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

        std::cout << "message received:" << message.message() << std::endl;

        // acknowledge the message
//        channel.ack(deliveryTag);
    };


    channel.consume("", AMQP::noack)
        .onReceived(messageCb)
        .onSuccess(startCb)
        .onError(errorCb);

    std::cout << " [*] Waiting for messages. To exit press CTRL-C\n";

    ioService.run();
    return 0;
}
