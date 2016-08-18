/*
 * server.h
 *
 *  Created on: 2016-8-5
 *      Author: root
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "bingo/string.h"
#include "bingo/singleton.h"
#include "bingo/thread/all.h"
using namespace bingo::thread;
using namespace bingo;


#include "bingo/tcp/all.h"
using namespace bingo::tcp;

#include <fstream>
#include <iostream>
using namespace std;

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;
ptime p_start, p_end;

#define EVERY_READ_DATA_LENGTH 1024
int rev_amount = 0;





// ----------------------------- TCP_MESSAGE_PACKAGE ------------------------------ //
#pragma pack(1)
struct my_package{
	char data[1042];
};
#pragma pack()


// ----------------------------- PARSER ------------------------------ //
struct my_parse{
	static const size_t header_size;								// Parse size of package's header.
	static int max_wait_for_heartjump_seconds;						// If the value is 0, then server don't check heartjump.
	static int max_wait_for_authentication_pass_seconds;			// If the value is 0, then server don't check authentication pass.


};
const size_t my_parse::header_size = 5;
int my_parse::max_wait_for_heartjump_seconds = 0;
int my_parse::max_wait_for_authentication_pass_seconds = 0;

// ----------------------------- SOCKET_MANAGER ------------------------------ //
class my_handler;
typedef bingo::singleton_v0<tcp_svr_hdr_manager<my_handler> > my_mgr;

// ----------------------------- HANDLER ------------------------------ //
//typedef tcp_svr_handler<my_parse,			// Define static header_size
//	 	 				my_package			// Message
//	 	 	> my_handler;

struct my_info{
	char data[1042];
	size_t data_size;
};

vector<my_info*> my_vect;



// --------------------------- message ------------------------- //

#pragma pack(1)
struct mypackage{
	char message[1042];
};
#pragma pack()

typedef task_message_data<mypackage> 	my_data_message;
typedef task_exit_data				    my_exit_message;

// --------------------------- many_to_one ------------------------- //

typedef many_to_one<my_data_message,
					my_exit_message
	> my_task_by_thread;
typedef bingo::singleton_v1<my_task_by_thread, my_task_by_thread::thr_top_callback> my_task;

void top(my_data_message*& msg){

//	string_ex t;
//	message_out_with_thread("top data:" << t.stream_to_string(&msg->data.object()->message[0], 4))

	my_info* info = new my_info();
	memcpy(info->data, msg->data.header(), msg->data.length());
	info->data_size = msg->data.length();
	my_vect.push_back(info);
}



void write_file(){

	ofstream fout("/opt/src/example2.tar.gz",ios::binary);

	char w[EVERY_READ_DATA_LENGTH];
	memset(&w[0], 0x00, EVERY_READ_DATA_LENGTH);

	for (int i = 0; i < my_vect.size(); ++i) {
		my_info* info = my_vect[i];

		char* data = info->data;

		size_t w_size = 0;
		memcpy(&w_size, data + 3, 2);
		memset(&w[0], 0x00, EVERY_READ_DATA_LENGTH);
		memcpy(&w[0], data + 3 + 2, w_size);

		fout.write(w, w_size);
	}

	fout.close();

//	bingo::string_ex t;
//	cout << "get data:" << t.stream_to_string(w, w_size) << endl;

}


class my_handler : public tcp_svr_handler<my_parse, my_package>{
public:
	my_handler(boost::asio::io_service& io_service) :
		tcp_svr_handler<my_parse,my_package>(io_service){

	}

	int read_pk_header_complete_func(
					my_handler::pointer p,
					char*& rev_data,
					size_t& rev_data_size,
					size_t& remain_size,
					u8_t& err_code){

		u16_t size = 0;
		memcpy(&size, rev_data + 3, 2);

		remain_size = size + 4 + 4 + 4 + 1;

		return 0;
	}

	int read_pk_full_complete_func(
				my_handler::pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				u8_t& err_code){

		string_ex t;
//		cout << "hdr:" << p.get() << ",rev_amount:" << rev_amount <<
//				",get data:" <<  t.stream_to_string(rev_data, rev_data_size) << endl;

		rev_amount++;

		cout << "hdr:" << p.get() << ",rev_amount:" << rev_amount << endl;

		if(rev_amount == 1)
			p_start = second_clock::local_time();
		else
			p_end = second_clock::local_time();

//		{
//			my_info* info = new my_info();
//			memcpy(info->data, rev_data, rev_data_size);
//			info->data_size = rev_data_size;
//			my_vect.push_back(info);
//		}

		{
			my_data_message* msg = new my_data_message();
			msg->data.copy(rev_data, rev_data_size, err_code);

			int e_code = 0;
			if(my_task::instance()->put(msg, e_code) == -1){			// Input T into queue

				string_ex t;
				message_out_with_thread("put error, err_code:" << err_code << ",data:" << t.stream_to_string(&msg->data.object()->message[0], 4))

				delete msg;

			}else{

//				string_ex t;
//				message_out_with_thread("put success, data:" << t.stream_to_string(&msg->data.object()->message[0], 4))
			}
		}



		if(rev_amount == 1043850){

			this_thread::sleep(seconds(2));

			time_duration td_start = p_start.time_of_day();
			time_duration td_end   = p_end.time_of_day();

			time_duration td = td_end - td_start;


			cout << "total seconds:" << td.total_seconds() << endl;

			message_out_with_time("write file start!")
			write_file();
			message_out_with_time("write file end!")
		}

		return 0;
	}


	void write_pk_full_complete_func(
				pointer p,
				char*& snd_p,
				size_t& snd_size,
				const boost::system::error_code& ec){
	}



	void catch_error_func(my_handler::pointer p, u8_t& err_code){
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





// ----------------------------- SERVER ------------------------------ //

vector<void*> handler_pointers;

class my_server : public tcp_server<my_handler, my_mgr, my_parse>{
public:
	my_server(boost::asio::io_service& io_service, string& ipv4, u16_t& port):
		tcp_server<my_handler, my_mgr, my_parse>(io_service, ipv4, port){

	}

	int accept_success_func(my_server::pointer ptr, u8_t& err_code){

		my_mgr::instance()->push(ptr.get());
//		cout << "hdr:" << ptr.get() << ",do accept_success_func() success" << endl;
		handler_pointers.push_back(ptr.get());
		return 0;
	}
};
//typedef tcp_server<my_handler, my_mgr, my_parse> my_server;




#endif /* SERVER_H_ */
