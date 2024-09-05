#include "Resources_manager.hpp"


ResourcesManager::ResourcesManager()
{
    // scan the Resources directory named "data"
    // for each file in the directory
    // create a Resource object and add it to the Resources vector
    std::string path = "data";
    for (const auto & entry : std::filesystem::directory_iterator(path))
    {
        Resources.push_back(std::make_shared<Resource>(entry.path().filename().string()));
    }
}

ResourcesManager::~ResourcesManager()
{
}

ResourcesManager *ResourcesManager::instance_ = nullptr;

ResourcesManager *ResourcesManager::getInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new ResourcesManager();
    }
    return instance_;
}

std::shared_ptr<Resource> ResourcesManager::getResource(std::string ResourceName)
{
    for (auto Resource : Resources)
    {
        if (Resource->getName() == ResourceName)
        {
            if (Resource->getStream() == nullptr)
            {
                Resource->setStream(new std::ifstream("data/" + ResourceName));
            }
            return Resource;
        }
    }
    return nullptr;
}

void ResourcesManager::closeUnusedResources()
{
    for (auto Resource : Resources)
    {
        if (Resource.use_count() == 1)
        {
            Resource->closeStream();
        }
    }
}

Resource::Resource(std::string name)
{
    this->name = name;
}

Resource::~Resource()
{
}

std::string Resource::getName()
{
    return this->name;
}

std::istream *Resource::getStream()
{
    return this->stream;
}

void Resource::setStream(std::istream *stream)
{
    this->stream = stream;
}

void Resource::closeStream()
{
    if (stream != nullptr)
    {
        delete stream;
    }
}
