/*
 * main.cpp
 *
 *  Created on: 2016-8-17
 *      Author: root
 */

#include <unistd.h> // sleep()

#include <iostream>
#include <fstream>
using namespace std;

#include <boost/thread.hpp>
using namespace boost;

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;


int main (int argc, char *argv[]) {


	for (int i = 0; i < 1000; ++i) {
		ofstream myfile("example.txt",ios_base::app);  //example.txt是你要输出的文件的名字
		 if(!myfile)
		 {
			 cout<<"error !";
		 }
		 else
		 {

				date now = day_clock::local_day();
				int year = now.year();
				int month = now.month();
				int day = now.day();

				ptime p1 = second_clock::local_time();
				int hour = p1.time_of_day().hours();
				int minute = p1.time_of_day().minutes();
				int second = p1.time_of_day().seconds();

				string dt;
				char cnow[30];
				memset(&cnow[0], 0x00, 30);
				sprintf(&cnow[0], "%4d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
				dt.append(cnow);

				 myfile<<"My first out! -- "  << dt << endl;
				 myfile.close();
		 }

		 sleep(1);
//		 this_thread::sleep(milliseconds(100));

	}

	return 0;
}


