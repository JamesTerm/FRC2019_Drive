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
				static void General(string message, bool toDriverStation = false);
				static void Warning(string message);
				static void restartfile();
				static void closeLogFile();
			private:
				//static const string filename;
				//static ofstream file_;
				static void Append(string message);				
    	};

}

#endif