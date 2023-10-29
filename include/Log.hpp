#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class Logger
{
public:
    Logger();
    ~Logger();
    void Log(string message);
private:
    string log_file_path = "log.txt";
    FILE* log_file;
};