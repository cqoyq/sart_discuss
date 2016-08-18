/*
 * my.h
 *
 *  Created on: 2016-8-4
 *      Author: root
 */

#ifndef MY_H_
#define MY_H_

#include "view_callback.h"

class my : public view_callback {
public:
	my();
	virtual ~my();

	void call(void*& data);
};

#endif /* MY_H_ */
