#include <LoadDatas.hpp>

DataLoader::DataLoader(const std::string &ldtk_file_path)
{
    std::ifstream file(ldtk_file_path);
    if (file.is_open())
    {
        file >> ldtkjs;
    }
    else
    {
        std::cerr << "Error: Could not open file " << ldtk_file_path << std::endl;
    }

    file.close();
}

DataLoader::~DataLoader()
{
}

LdtkJson DataLoader::get_ldtk_json()
{
    return this->ldtkjs;
}