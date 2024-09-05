#pragma once
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class Logger
{
    protected:
        Logger();
        static Logger* instance_;
    public:
        static Logger* GetInstance();
        ~Logger();
        void log(string message);
    private:
        string log_file_path = "log.txt";
        FILE* log_file;
};