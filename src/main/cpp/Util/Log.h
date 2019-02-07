/****************************** Header ******************************\
Class Name: Log
File Name:	Log.h
Summary: Logs
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s): Ian Poll
Email: irobot9803@gmail.com
\********************************************************************/

#ifndef SRC_UTIL_LOG_H_
#define SRC_UTIL_LOG_H_

#include "../Config/ActiveCollection.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace Logger
{

	class Log
    	{
        	public:
				static void Error(string message);
				static void General(string message, bool toDriverStation);
				static void General(string message);
				static void Warinig(string message);
				static void restartfile();
			private:
				//static const string filename;
				//static ofstream file_;
				static void Append(string message);				
    	};

}

#endif