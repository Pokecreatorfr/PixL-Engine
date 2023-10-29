#include <Log.hpp>

Logger::Logger()
{
    log_file = fopen(log_file_path.c_str(),"w");
}

Logger::~Logger()
{
    fclose(log_file);
}

void Logger::Log(string message)
{
    fprintf(log_file,"%s\n",message.c_str());
    cout << message << endl;
}
