/***********************************************************************
 * Module:  view_callback.h
 * Author:  Administrator
 * Modified: 2016年8月1日 10:53:57
 * Purpose: Declaration of the class view_callback
 ***********************************************************************/


#if !defined(__COM_SART_BIZ_VIEW_CALLBACK_H)
#define __COM_SART_BIZ_VIEW_CALLBACK_H

#include <string>
#include <string.h>
using namespace std;

class view_callback
{
public:
   /* View callback function */
   virtual void call(void*& data)=0;

protected:
private:

};

#endif
