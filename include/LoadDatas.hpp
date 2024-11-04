#pragma once
// #include <LdtkJson.hpp>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

class LdtkJson;

class DataLoader
{
public:
    DataLoader(const std::string &ldtk_file_path);
    ~DataLoader();

    LdtkJson get_ldtk_json();

private:
    LdtkJson ldtkjs;
};

namespace ldtk
{

    class LdtkJson
    {
    public:
        LdtkJson(const std::string &ldtk_file_path);
        ~LdtkJson();

    private:
        std::string bgColor;
        bool externalLevels;
        std::string iid;
        std::string jsonVersion;
    };

    class defs
    {
    public:
        defs();
    };

    class entitiesDefs
    {
    public:
        entitiesDefs();

    private:
        std::string bgColor;
        int height;
        std::string identifier;
        // int nineSliceBorder;
        float pivotX;
        float pivotY;
        TileRect tileRect;
    };

    struct TileRect
    {
        int h;
        int tileesetUid;
        int w;
        int x;
        int y;
    };
}