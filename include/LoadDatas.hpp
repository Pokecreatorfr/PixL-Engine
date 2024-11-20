#pragma once
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

class ldtk::LdtkJson;
enum ldtk::TileRenderMode;
struct ldtk::TilesetRect;
struct ldtk::EntitiesDefs;
struct ldtk::Enum;
struct ldtk::EnumValue;
struct ldtk::def;
struct ldtk::layerdef;
struct ldtk::IntGridValues;
struct ldtk::IntGridValueGroups;
struct ldtk::Tileset;
struct ldtk::TilesetCustomData;
struct ldtk::TilesetEnumTags;
struct ldtk::Level;
struct ldtk::BgPos;
struct ldtk::Neighbour;
struct ldtk::FieldInstance;
struct ldtk::LayerInstance;
struct ldtk::EntityInstance;
struct ldtk::TileInstance;
#include <variant>

namespace ldtk
{

    // classes

    struct LdtkJson
    {
        std::string bgColor;
        def defs;
        bool externalLevels;
        std::string iid;
        std::string jsonVersion;
        std::vector<Level> levels;
        std::vector<TocObject> toc;
        std::optional<int> worldGridHeight;
        std::optional<int> worldGridWidth;
        std::optional<WorldLayout> worldLayout;
        std::vector<World> worlds;
    };

    struct TocObject
    {
        std::string identifier;
        std::vector<InstanceData> instancesData;
        std::vector<EntityRef> instances;
    };

    struct InstanceData
    {
        // TODO : fields
        int heiPx;
        int iid;
        EntityRef iids;
        int widPx;
        int worldX;
        int worldY;
    };

    struct EntityRef
    {
        std::string entityIid;
        std::string layerIid;
        std::string levelIid;
        std::string worldIid;
    };

    struct World
    {
        std::string identifier;
        std::string iid;
        std::vector<Level> levels;
        int worldGridHeight;
        int worldGridWidth;
        WorldLayout worldLayout;
    };

    struct Level
    {
        std::string bgColor;
        std::optional<BgPos> __bgPos;
        std::vector<Neighbour> __neighbours;
        std::optional<std::string> bgRelPath;
        std::optional<std::string> externalRelPath;
        std::vector<FieldInstance> fieldInstances;
        std::string identifier;
        std::string iid;
        std::optional<std::vector<LayerInstance>> layerInstances;
        int pxHei;
        int pxWid;
        int uid;
        int worldDepth;
        int worldX;
        int worldY;
    };

    struct FieldInstance
    {
        std::string __identifier;
        std::optional<TilesetRect> __tile;
        std::string __type;
        std::variant<float, int, bool, std::string, int[2], TilesetRect, EntityRef> value;
        int defUid;
    };

    struct LayerInstance
    {
        int __cHei;
        int __cWid;
        int __gridSize;
        std::string __identifier;
        float __opacity;
        int __pxTotalOffsetX;
        int __pxTotalOffsetY;
        std::optional<int> __tilesetDefUid;
        std::optional<std::string> __tilesetRelPath;
        std::string __type;
        std::vector<TileInstance> autoLayerTiles;
        std::vector<EntityInstance> entityInstances;
        std::vector<TileInstance> gridTiles;
        std::string iid;
        std::vector<int> intGridCsv;
        int layerDefUid;
        int levelId;
        std::optional<int> overrideTilesetUid;
        int pxOffsetX;
        int pxOffsetY;
        bool visible;
        std::optional<std::vector<int>> intGrid;
    };

    struct EntityInstance
    {
        int __grid[2];
        std::string __identifier;
        float __pivot[2];
        std::string __smartColor;
        std::vector<std::string> __tags;
        std::optional<TilesetRect> __tile;
        std::optional<int> __worldX;
        std::optional<int> __worldY;
        int defUid;
        std::vector<FieldInstance> fieldInstances;
        int height;
        std::string iid;
        int px[2];
        int width;
    };

    struct TileInstance
    {
        float a;
        int f;
        int px[2];
        int src[2];
        int t;
    };

    struct Neighbour
    {
        std::string dir;
        std::string levelIid;
        std::optional<int> levelUid;
    };

    struct BgPos
    {
        float cropRect[4];
        float scale[2];
        int topLeftPx[2];
    };

    enum WorldLayout
    {
        Null,
        Free,
        GridVania,
        LinearHorizontal,
        LinearVertical,
    };

    struct def
    {
        std::vector<EntitiesDefs> entities;
        std::vector<Enum> enums;
        std::vector<Enum> externalEnums;
        std::vector<layerdef> layerDefs;
        std::vector<Tileset> tilesets;
    };

    enum TileRenderMode
    {
        Cover,
        FitInside,
        Repeat,
        Stretch,
        FullSizeCropper,
        FullSizeUncropped,
        NineSlice,
    };

    std::vector<std::string> TileRenderMode_str = {
        "Cover",
        "FitInside",
        "Repeat",
        "Stretch",
        "FullSizeCropper",
        "FullSizeUncropped",
        "NineSlice",
    };

    struct TilesetRect
    {
        int h;
        int tileesetUid;
        int w;
        int x;
        int y;
    };

    struct EntitiesDefs
    {
        std::string bgColor;
        int height;
        std::string identifier;
        int nineSliceBorder[4];
        float pivotX;
        float pivotY;
        std::optional<TilesetRect> tileRect;
        TileRenderMode tileRenderMode;
        std::optional<int> tilesetId;
        std::optional<TilesetRect> uiTileRect;
        int uid;
        int width;
        std::optional<int> tileId;
    };

    struct Enum
    {
        std::optional<std::string> externalRelPath;
        std::optional<int> iconTilesetUid;
        std::string identifier;
        std::vector<std::string> tags;
        int uid;
        std::vector<EnumValue> values;
    };

    struct EnumValue
    {
        int color;
        std::string id;
        std::optional<TilesetRect> tileRect;
        std::optional<int> tileId;
        std::optional<std::vector<int>> __tileSrcRect;
    };

    struct layerdef
    {
        std::string __type;
        std::optional<int> autoSourceLayerDefUid;
        float displayOpacity;
        int gridSize;
        std::string identifier;
        IntGridValues intGridValues;
        IntGridValueGroups intGridValueGroups;
        float parallaxFactorX;
        float parallaxFactorY;
        bool parallaxScaling;
        int pxOffsetX;
        int pxOffsetY;
        std::optional<int> tilesetDefUid;
        int uid;
        std::optional<int> autoTilesetDefUid;
    };

    struct IntGridValues
    {
        std::string color;
        int groupUid;
        std::optional<std::string> identifier;
        std::optional<TilesetRect> tile;
        int value;
    };

    struct IntGridValueGroups
    {
        std::optional<std::string> color;
        std::optional<std::string> identifier;
        int uid;
    };

    struct Tileset
    {
        int __cHei;
        int __cWid;
        std::vector<TilesetCustomData> customData;
        // TODO: embedAtlas
        std::vector<TilesetEnumTags> enumTags;
        std::string identifier;
        int padding;
        int pxHei;
        int pxWid;
        std::optional<std::string> relPath;
        int spacing;
        std::vector<std::string> tags;
        std::optional<int> tagsSourceTilesetUid;
        int tileGridSize;
        int uid;
    };

    struct TilesetCustomData
    {
        std::string data;
        int tileId;
    };

    struct TilesetEnumTags
    {
        std::string enumValueId;
        std::vector<int> tileIds;
    };

}

class DataLoader
{
public:
    DataLoader(const std::string &ldtk_file_path);
    ~DataLoader();

    ldtk::LdtkJson get_ldtk_json();

private:
    ldtk::LdtkJson *ldtkjs;

    ldtk::def load_defs(nlohmann::json json);
    std::vector<ldtk::EntitiesDefs> load_entities_defs(nlohmann::json json);
    ldtk::TilesetRect load_tileset_rect(nlohmann::json json);
    std::vector<ldtk::Enum> load_enum(nlohmann::json json);
    std::vector<ldtk::EnumValue> load_enum_value(nlohmann::json json);
    std::vector<ldtk::layerdef> load_layerdefs(nlohmann::json json);
    std::vector<ldtk::Tileset> load_tilesets(nlohmann::json json);
    std::vector<ldtk::Level> load_levels(nlohmann::json json);
    std::vector<ldtk::FieldInstance> load_field_instances(nlohmann::json json);
    std::vector<ldtk::LayerInstance> load_layer_instances(nlohmann::json json);
    std::vector<ldtk::TileInstance> load_tile_instances(nlohmann::json json);
    std::vector<ldtk::EntityInstance> load_entity_instances(nlohmann::json json);
};