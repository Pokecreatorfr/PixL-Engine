#pragma once
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class Logger
{
public:
    Logger();
    ~Logger();
    void log(string message);
private:
    string log_file_path = "log.txt";
    FILE* log_file;
};