#include <iostream>

#include "asiohandler.h"

#include "bingo/string.h"
#include "bingo/type.h"
using namespace bingo;

#include <string>
using namespace std;

#define max_wait_for_heartjump_seconds 90

bool is_heartbeat(char* data, size_t data_size){

	char heart[8] = {0x08, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0xce};

	if(data[0] == 0x08 && memcmp(heart, data, 8) == 0){
		return true;
	}else{
		return false;
	}
}


bool is_data_message(char* data, size_t data_size){

	char* header = data;
	u16_t app_size = (u8_t)(*(header + 5) )* 256 + (u8_t)(*(header + 6) );

	size_t size0 = 1 + 4 + 2 + app_size + 1;

//	cout << "size0:" << size0 << "data_size:" << data_size  << endl;
//	return false;

	if(size0 == data_size){

		cout << "size0:" << size0 << ",data_size:" << data_size  << endl;

		// one package.
		if(header[0] == 0x03 && header[2] == 0x01)
			return true;
		else
			return false;

	}else {

		// multi-package.
		while(size0 != data_size){
			header = data + size0;
			u16_t app_size = (u8_t)(*(header + 5) )* 256 + (u8_t)(*(header + 6) );

			if(header[0] == 0x03 && header[2] == 0x01)
					return true;

			size0 += 1 + 4 + 2 + app_size + 1;

			cout << "size0:" << size0 << ",data_size:" << data_size  << endl;
		}

		return false;
	}


}

string get_now(){
	string now;
	ptime p1 = boost::posix_time::microsec_clock::local_time();
	int year = p1.date().year();
	int month = p1.date().month();
	int day = p1.date().day();

	int hour = p1.time_of_day().hours();
	int minute = p1.time_of_day().minutes();
	int second = p1.time_of_day().seconds();

	int millsecond = p1.time_of_day().total_milliseconds() - p1.time_of_day().total_seconds()*1000;

	char ctime[20];
	memset(&ctime[0], 0x00, 20);

	sprintf(&ctime[0], "%4d-%02d-%02d %02d:%02d:%02d.%03d", year, month, day, hour, minute, second, millsecond);
	now.append(ctime);

	return now;
}

using boost::asio::ip::tcp;

class AmqpBuffer
{
public:
    AmqpBuffer(size_t size) :
            _data(size, 0),
            _use(0)
    {
    }

    size_t write(const char* data, size_t size)
    {
        if (_use == _data.size())
        {
            return 0;
        }

        const size_t length = (size + _use);
        size_t write = length < _data.size() ? size : _data.size() - _use;
        memcpy(_data.data() + _use, data, write);
        _use += write;
        return write;
    }

    void drain()
    {
        _use = 0;
    }

    size_t available() const
    {
        return _use;
    }

    const char* data() const
    {
        return _data.data();
    }

    void shl(size_t count)
    {
        assert(count < _use);

        const size_t diff = _use - count;
        std::memmove(_data.data(), _data.data() + count, diff);
        _use = _use - count;
    }

private:
    std::vector<char> _data;
    size_t _use;
};

AsioHandler::AsioHandler(boost::asio::io_service& ioService) :
        _ioService(ioService),
        _socket(ioService),
        _timer(ioService),
        _asioInputBuffer(ASIO_INPUT_BUFFER_SIZE, 0),
        _amqpBuffer(new AmqpBuffer(ASIO_INPUT_BUFFER_SIZE * 2)),
        _connection(nullptr),
        _writeInProgress(false),
        _connected(false),
        _quit(false),
        heartjump_timer_(ioService)
{
}

AsioHandler::~AsioHandler()
{
}

void AsioHandler::connect(const std::string& host, uint16_t port)
{
    doConnect(host, port);
}

void AsioHandler::check_heartjump(){

			boost::posix_time::seconds s(max_wait_for_heartjump_seconds);
			boost::posix_time::time_duration td = s;
			heartjump_timer_.expires_from_now(td);
			heartjump_timer_.async_wait([this](const boost::system::error_code& ec){
				if(!ec){

					cout << get_now() << ", start to check heartjump!" << endl;
					if(check_heartjump_timeout()){
						send_heartjump();
						set_heartjump_timer();
					}

					check_heartjump();
				}
			});
}

bool AsioHandler::check_heartjump_timeout(){
	ptime now = boost::posix_time::microsec_clock::local_time();
	ptime p1 = now - seconds(max_wait_for_heartjump_seconds);
	if(p1_ < p1){
		return true;
	}else{
		return false;
	}
}

void AsioHandler::set_heartjump_timer(){
	p1_ = boost::posix_time::microsec_clock::local_time();
	cout << get_now() << ", call set_heartjump_timer()" << endl;
}

void AsioHandler::send_heartjump(){
	char heart[8] = {0x08, 0x00, 0x00, 0x00,
															0x00, 0x00, 0x00, 0xce};
	 boost::asio::async_write(_socket,
				boost::asio::buffer(&heart[0], 8),
				[this](boost::system::error_code ec, std::size_t length )
				{
					 if(!ec)
					{
						 cout << get_now() << ", send heart-beat to server!data_size:" << length << endl;
					}
				}
	 );
}

void AsioHandler::doConnect(const std::string& host, uint16_t port)
{
    tcp::resolver::query query(host, std::to_string(port));
    tcp::resolver::iterator iter = tcp::resolver(_ioService).resolve(query);
    _timer.expires_from_now(boost::posix_time::seconds(15));
    _timer.async_wait([this](const boost::system::error_code& ec){
        if(!ec && !_connected)
        {
            std::cerr<<"Connection timed out";
            _socket.cancel();
            exit(1);
        }
    });

    boost::asio::async_connect(_socket, iter,
            [this](boost::system::error_code ec, tcp::resolver::iterator)
            {
                _connected = true;
                if (!ec)
                {
                	// Connect success to check heart-jump.
                	{
                		set_heartjump_timer();
                		check_heartjump();
                	}

                    doRead();

                    if(!_outputBuffer.empty())
                    {
                        doWrite();
                    }
                }
                else
                {
                    std::cerr<<"Connection error:"<<ec<<std::endl;
                    exit(1);
                }
            });

}

void AsioHandler::onData(
        AMQP::Connection *connection, const char *data, size_t size)
{
    _connection = connection;

    _outputBuffer.push_back(std::vector<char>(data, data + size));
    if (!_writeInProgress && _connected)
    {
        doWrite();
    }
}

void AsioHandler::doRead()
{
    _socket.async_receive(boost::asio::buffer(_asioInputBuffer),
            [this](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    _amqpBuffer->write(_asioInputBuffer.data(), length);

                    {
                    	char out[2048] = {0x00};
                    	memcpy(&out[0], _asioInputBuffer.data(), length);
//                    	cout << "s:" << out << endl;

                    	bingo::string_ex t;
                    	std::cout  << get_now() <<", rev_data:" << t.stream_to_string(_asioInputBuffer.data(), length) << "," << length << std::endl;
                    }

                    {
                    	if(is_heartbeat(_asioInputBuffer.data(), length)){
                    		set_heartjump_timer();
                    	}
                    }
//
//                    {
//                    	if(is_data_message(_asioInputBuffer.data(), length)){
//
//                    	}
//                    }

                    parseData();
                    doRead();
                }
                else
                {
                    std::cerr<<"Error reading:"<<ec<<std::endl;
//                    exit(1);
                    _ioService.stop();
                }
            });
}

void AsioHandler::doWrite()
{
    _writeInProgress = true;
    boost::asio::async_write(_socket,
            boost::asio::buffer(_outputBuffer.front()),
            [this](boost::system::error_code ec, std::size_t length )
            {
                if(!ec)
                {

                	cout << get_now() << ", send data to server!data_size:" << length << endl;

                    _outputBuffer.pop_front();
                    if(!_outputBuffer.empty())
                    {
                        doWrite();
                    }
                    else
                    {
                        _writeInProgress = false;
                    }

                    if(_quit)
                    {
                        _socket.close();
                    }
                }
                else
                {
                    std::cerr<<"Error writing:"<<ec<<std::endl;
                    _socket.close();
                    exit(1);
                }
            });
}

void AsioHandler::parseData()
{
    if (_connection == nullptr)
    {
        return;
    }

    const size_t count = _connection->parse(_amqpBuffer->data(),
            _amqpBuffer->available());

//    {
//    	std::cout  << "parseData count:" << count << ",available:" << _amqpBuffer->available() << std::endl;
//    }

    if (count == _amqpBuffer->available())
    {
        _amqpBuffer->drain();
    }
    else if (count > 0)
    {
        _amqpBuffer->shl(count);
    }
}

void AsioHandler::onConnected(AMQP::Connection *connection)
{
}
bool AsioHandler::connected() const
{
    return _connected;
}

void AsioHandler::onError(AMQP::Connection *connection, const char *message)
{
//    std::cerr << "AMQP error " << message << std::endl;
    std::cout << "AMQP error " << message << std::endl;
}

void AsioHandler::onClosed(AMQP::Connection *connection)
{
    std::cout << "AMQP closed connection" << std::endl;
    _quit = true;
    if (!_writeInProgress)
    {
        _socket.close();
    }
}
