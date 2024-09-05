#include <logger.hpp>

Logger::Logger()
{
    log_file = fopen(log_file_path.c_str(),"w");
}

Logger* Logger::instance_ = nullptr;

Logger* Logger::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new Logger();
    }
    return instance_;
}

Logger::~Logger()
{
    fclose(log_file);
}

void Logger::log(string message)
{
    fprintf(log_file,"%s\n",message.c_str());
    #ifdef DEBUG
    cout << message << endl;
    #endif    
    fclose(log_file);
    log_file = fopen(log_file_path.c_str(),"a");
}