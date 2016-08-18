/*
 * main.cpp
 *
 *  Created on: 2016-8-18
 *      Author: root
 */

#include <memory.h>

#include <strstream>

#include <iostream>
#include <fstream>
using namespace std;

#include <boost/thread.hpp>
using namespace boost;

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;


bool is_start = false;

void loop(FILE *fstream){
	  char buff[1024];
	  memset (buff ,'\0', sizeof(buff));

	 if(NULL == fgets (buff, sizeof(buff), fstream))
	{
		pclose(fstream);

		cout << "command fgets fail!" << endl;
		return ;
	}
   string ss = buff;

   if(is_start) cout << "data:" << ss;
}

void CalculateDirUsedCapacity ()
{
    FILE *fstream = NULL;


    std::string cmd = "inotail -f -n 1 /opt/src/sart/discuss/04.create_info/example.txt";

    // 通过管道来获取系统命令返回的值
    if(NULL == (fstream = popen (cmd.c_str (), "r"))) {
//        return 0;
    	cout << "command popen fail!" << endl;
    	return;
    }

    while(true){
    	loop(fstream);
    }

//    int64_t usedCapacity = 0;
//    ss >> usedCapacity;
    pclose(fstream);  // close pipe

//    return usedCapacity;
}

void wait(){
	cout << "block start" << endl;
	this_thread::sleep(seconds(5));
	cout << "block end" << endl;
	is_start = true;
}

int main (int argc, char *argv[]) {
	// Start to wait for output.
	thread t1(CalculateDirUsedCapacity);

	// Block for 2 seconds.
	thread t2(wait);

	t1.join();
	t2.join();

	return 0;
}


