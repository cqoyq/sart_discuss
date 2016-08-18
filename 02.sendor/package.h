/*
 * package.h
 *
 *  Created on: 2016-8-5
 *      Author: root
 */

#ifndef PACKAGE_H_
#define PACKAGE_H_

#include <string.h>

#include "bingo/type.h"
using namespace bingo;

u32_t inext = 0;
u32_t inext_num = 1;

//				   header  type  frame length  application  next next_num  crc  tailer
#define total_size 1     + 1   + 1   + 2     + 1024       + 4  + 4       + 4  + 1
char pk[total_size];


void pack(char* data, size_t data_size, char*& out, size_t& out_size){

	out = &pk[0];
	out_size = 1     + 1   + 1   + 2     + data_size       + 4  + 4       + 4  + 1;

	memset(&pk[0], 0x00, total_size);

	pk[0] = 0x69;
	pk[1] = 0x10;
	pk[2] = 0x01;
	memcpy(pk + 3, &data_size, 2);

	memcpy(pk + 3 + 2, data, data_size);

	memcpy(pk + 3 + 2 + data_size, &inext, 4);
	memcpy(pk + 3 + 2 + data_size + 4, &inext_num, 4);

	pk[3 + 2 + data_size + 4 + 4 + 4] = 0x16;
}

#endif /* PACKAGE_H_ */
