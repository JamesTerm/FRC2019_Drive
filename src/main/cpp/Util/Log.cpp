/****************************** Header ******************************\
Class Name:	Log
File Name:	Log.cpp
Summary: 	Logging
Project:     BroncBotzFRC2019
Copyright (c) BroncBotz.
All rights reserved.

Author(s):	Ian Poll
Email:	irobot9803@gmail.com
\*********************************************************************/

#include "Log.h"
#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;
using namespace Logger;

    const string filename = "RoboLog.txt"; //name of file to be created
    static ofstream file_(filename);

    void Log::Error(string message)
    {
		cout << "ERROR: " + message << endl;
        Log::Append("ERROR: " + message);
        DriverStation::ReportError("ERROR: " + message);
	}

	void Log::General(string message, bool toDriverStation)
    {
		cout << message << endl;
		Log::Append(message);
		if(toDriverStation)
        {
			DriverStation::ReportWarning(message);
		}
	}
    void Log::General(string message)
    {
		cout << message << endl;
		Log::Append(message);
	}

	void Log::Warinig(string message)
    {
		cout << "WARNING: " + message << endl;
		Log::Append("WARNING: " + message);
		DriverStation::ReportWarning("WARNING: " + message);
	}

    void Log::restartfile()
    {
        file_.open(filename, ios::app | ios::trunc); //opens file then clears it (trunc) then appends things in it (app)
    }

    void Log::Append(string message)
    {
        if(file_)
        {
            file_ << message << endl; //appends message in file
        }   
    }
