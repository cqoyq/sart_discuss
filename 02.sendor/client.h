/*
 * client.h
 *
 *  Created on: 2016-8-5
 *      Author: root
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "bingo/tcp/all.h"
using namespace bingo::tcp;

int snd_amount = 0;

// ----------------------------- HEARTJUMP_PACKAGE ------------------------------ //

#pragma pack(1)
struct my_heartjump{
	static size_t data_size;
	static char data[11];
};
#pragma pack()
size_t 	my_heartjump::data_size = 11;
char 	my_heartjump::data[11] = {0x68, 0x23, 0x00, 0x00, 0x01, 0x00, 0x8f, 0x72, 0xd8, 0x0d, 0x16};

// ----------------------------- TCP_MESSAGE_PACKAGE ------------------------------ //

#pragma pack(1)
struct my_package{
	char data[1042];
};
#pragma pack()

// ----------------------------- PARSER ------------------------------ //
struct my_parse{
	static int retry_delay_seconds;				// When Client connect fail, don't reconnect if the value is 0.
	static int max_retry_delay_seconds;

	static const size_t header_size;				// Parse size of package's header.
	static int max_interval_seconds_on_heartjump;	// Client don't send heartjump if the value is 0.
};
int my_parse::retry_delay_seconds = 3;
int my_parse::max_retry_delay_seconds = 60;

const size_t my_parse::header_size = 5;
int my_parse::max_interval_seconds_on_heartjump = 0;

// ----------------------------- SOCKET_MANAGER ------------------------------ //
class my_handler;
typedef bingo::singleton_v0<tcp_cet_hdr_manager<my_handler> > my_mgr;

// ----------------------------- HANDLER ------------------------------ //

class my_handler : public tcp_cet_handler<my_heartjump, my_parse, my_package>{
public:
	my_handler(boost::asio::io_service& io_service, function<void()> f) :
		tcp_cet_handler<my_heartjump, my_parse, my_package>(io_service, f){

	}

	int read_pk_header_complete_func(
				pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				size_t& remain_size,
				int& err_code){
		u16_t size = 0;
		memcpy(&size, rev_data + 3, 2);

		remain_size = size + 4 + 4 + 4 + 1;

		return 0;
	}

	int read_pk_full_complete_func(
				pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				int& err_code){

		bingo::string_ex t;
		cout << "hdr:" << p.get() << ",read message:" <<
				t.stream_to_string(rev_data, rev_data_size) << endl;

		return 0;
	}

	void write_pk_full_complete_func(
			pointer p,
			char*& snd_p,
			size_t& snd_size,
			const boost::system::error_code& ec){

		snd_amount++;

//		bingo::string_ex t;
//		cout << "hdr:" << p.get() << ",snd_amount:" << snd_amount << ",write message:" <<
//				t.stream_to_string(snd_p, snd_size) << endl;

//		cout << "hdr:" << p.get() << ",snd_amount:" << snd_amount <<  endl;
	}



	void catch_error_func(pointer p, int& err_code){
		cout << "hdr:" << p.get() << ",err_code:" << (int)err_code << ",do catch_error()" << endl;
	}

	void close_complete_func(my_handler::pointer p, int& ec_value){
		u8_t err_code = 0;
		if(my_mgr::instance()->erase(p.get(), err_code) == 0){
//			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),success" << endl;
		}else{
//			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),err_code:" << err_code << endl;
		}
	}
};


// ----------------------------- CLIENT ------------------------------ //

void* handler_pointer = 0;

class my_client : public tcp_client<my_handler, my_parse> {
public:
	my_client(boost::asio::io_service& io_service, string& ipv4, u16_t& port):
		tcp_client<my_handler, my_parse>(io_service, ipv4, port){

	}

	int connet_success_func(pointer ptr, u8_t& err_code){
		message_out_with_time("hdr:" << ptr.get() << ",connect success!")

		my_mgr::instance()->push(ptr.get());
		handler_pointer = ptr.get();
		return 0;
	}

	void connet_fail_func(pointer ptr, int& retry_delay_seconds){
		cout << "hdr:" << ptr.get() << ",reconnect after " << retry_delay_seconds << " seconds." << endl;
	}
};




void run_client(char* data, size_t data_size){

//	this_thread::sleep(seconds(4));

	u8_t err_code = 0;
	my_mgr::instance()->send_data(handler_pointer, data, data_size, err_code);
};

#endif /* CLIENT_H_ */
