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

    void Log::General(string message, bool toDriverStation)
    {
        if(toDriverStation)
        {
            cout << "WARNING LOG:" + message << endl;
        }
        Log::Append("log: " + message);
	}

    void Log::Error(string message)
    {
		cout << "ERROR: " + message << endl;
        Log::Append("ERROR: " + message);
	}

	void Log::Warning(string message)
    {
		cout << "WARNING: " + message << endl;
		Log::Append("WARNING: " + message);
	}

    void Log::restartfile()
    {
        file_.open(filename);
        //file_.open(filename, ios::app | ios::trunc); //opens file then clears it (trunc) then appends things in it (app)
        cout << "File " + filename + " created" << endl;
    }

    void Log::Append(string message)
    {
        if(file_)
        {
            file_ << message << endl; //appends message in file
        }   
    }
