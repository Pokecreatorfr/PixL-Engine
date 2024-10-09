#include <LdtkJson.hpp>
#include <iostream>
#include <fstream>
#include <string>

class DataLoader {
public:
    DataLoader(const std::string& ldtk_file_path);
    ~DataLoader();

    quicktype::LdtkJson get_ldtk_json() const;

private:
    quicktype::LdtkJson ldtkjs;
};