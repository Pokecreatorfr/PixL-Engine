#include "LoadDatas.hpp"

DataLoader::DataLoader(const std::string &ldtk_file_path)
{
    std::ifstream file(ldtk_file_path);

    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << ldtk_file_path << std::endl;
        exit(1);
    }

    nlohmann::json jsonData = nlohmann::json::parse(file);

    from_json(jsonData, ldtkjs);
}

DataLoader::~DataLoader()
{
}

quicktype::LdtkJson DataLoader::get_ldtk_json() const
{
    return ldtkjs;
}