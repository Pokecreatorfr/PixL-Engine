#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <memory>

class Resource{
    public:
        Resource( std::string name);
        ~Resource();
        std::string getName();
        std::istream* getStream();
        void setStream(std::istream* stream);
        void closeStream();
    private:
        std::string name;
        std::istream* stream = nullptr;
};


class ResourcesManager
{
    protected:
        ResourcesManager();
        ~ResourcesManager();
        static ResourcesManager* instance_;
    public:
        static ResourcesManager* getInstance();
        // return io stream from file to read 
        std::shared_ptr<Resource> getResource(std::string ResourceName);
        void closeUnusedResources();
    private:
        std::vector<std::shared_ptr<Resource>> Resources;
};