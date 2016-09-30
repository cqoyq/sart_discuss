#pragma once

#include <deque>
#include <vector>
#include <memory>

#include <amqpcpp.h>
#include <boost/asio.hpp>

#include <boost/bind.hpp>
using namespace boost;
using namespace boost::asio;

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

class AmqpBuffer;
class AsioHandler: public AMQP::ConnectionHandler
{
private:

    typedef std::deque<std::vector<char>> OutputBuffers;

    virtual void onData(AMQP::Connection *connection, const char *data, size_t size);
    virtual void onConnected(AMQP::Connection *connection);
    virtual void onError(AMQP::Connection *connection, const char *message);
    virtual void onClosed(AMQP::Connection *connection);

    void doConnect(const std::string& host, uint16_t port);

    void doRead();

    void doWrite();

    void parseData();

private:

    boost::asio::io_service& _ioService;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::deadline_timer _timer;

    std::vector<char> _asioInputBuffer;
    std::shared_ptr<AmqpBuffer> _amqpBuffer;
    AMQP::Connection* _connection;
    OutputBuffers _outputBuffer;
    bool _writeInProgress;
    bool _connected;
    bool _quit;

public:

    static constexpr size_t ASIO_INPUT_BUFFER_SIZE = 4*1024; //4kb

    AsioHandler(boost::asio::io_service& ioService);

    void connect(const std::string& host, uint16_t port);

    virtual ~AsioHandler();

    AsioHandler(const AsioHandler&) = delete;
    AsioHandler& operator=(const AsioHandler&)=delete;

    bool connected()const;

private:
    ptime p1_;						// Save heart jump datetime.
    deadline_timer heartjump_timer_;

    void check_heartjump();
    bool check_heartjump_timeout();
    void send_heartjump();
    void set_heartjump_timer();
};

