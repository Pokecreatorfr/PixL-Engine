#include <Save.hpp>

Save::Save()
{
}

Save* Save::instance_ = nullptr;

Save* Save::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new Save();
    }
    return instance_;
}

std::vector<int> Save::GetKeys()
{
    return this->keys;
}