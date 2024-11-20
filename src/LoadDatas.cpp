#include <LoadDatas.hpp>

DataLoader::DataLoader(const std::string &ldtk_file_path)
{
    std::ifstream file(ldtk_file_path);
    std::string stdjson;

    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            stdjson += line;
        }
        file.close();
    }
    else
    {
        std::cout << "Error: Unable to open file" << std::endl;
    }

    nlohmann::json json = nlohmann::json::parse(stdjson);
    ldtk::LdtkJson *ldtkjson = new ldtk::LdtkJson();
    // bgColor
    ldtkjs->bgColor = json["bgColor"].get<std::string>();
    // defs
    ldtkjs->defs = load_defs(json["defs"]);
    // externalLevels
    ldtkjs->externalLevels = json["externalLevels"].get<bool>();
    // iid
    ldtkjs->iid = json["iid"].get<std::string>();
    // jsonVersion
    ldtkjs->jsonVersion = json["jsonVersion"].get<std::string>();
    // levels
    ldtkjs->levels = load_levels(json["levels"]);
}

DataLoader::~DataLoader()
{
    delete this->ldtkjs;
}

ldtk::def DataLoader::load_defs(nlohmann::json json)
{
    ldtk::def defs;
    // entities
    defs.entities = load_entities_defs(json["entities"]);
    // enums
    defs.enums = load_enum(json["enums"]);
    // externalEnums
    defs.externalEnums = load_enum(json["externalEnums"]);
    // layerDefs
    defs.layerDefs = load_layerdefs(json["layerDefs"]);
    // tilesets
    defs.tilesets = load_tilesets(json["tilesets"]);
    return defs;
}

std::vector<ldtk::EntitiesDefs> DataLoader::load_entities_defs(nlohmann::json json)
{
    std::vector<ldtk::EntitiesDefs> entities;
    for (auto &entity : json)
    {
        ldtk::EntitiesDefs entitydef;
        // color
        entitydef.bgColor = entity["color"].get<std::string>();
        // height
        entitydef.height = entity["height"].get<int>();
        // identifier
        entitydef.identifier = entity["identifier"].get<std::string>();
        // nineSliceBorder
        for (int i = 0; i < 4; i++)
        {
            entitydef.nineSliceBorder[i] = entity["nineSliceBorder"][i].get<int>();
        }
        // pivotX
        entitydef.pivotX = entity["pivotX"].get<float>();
        // pivotY
        entitydef.pivotY = entity["pivotY"].get<float>();
        // tileRect
        if (entity.find("tileRect") != entity.end())
        {
            entitydef.tileRect = load_tileset_rect(entity["tileRect"]);
        }
        else
        {
            entitydef.tileRect = std::nullopt;
        }
        // tileRenderMode (enum)
        // get const char* from json
        const char *tileRenderMode = entity["tileRenderMode"].get<std::string>().c_str();
        // find the index of the const char* in the vector
        auto it = std::find(ldtk::TileRenderMode_str.begin(), ldtk::TileRenderMode_str.end(), tileRenderMode);
        // get the index of the const char* in the vector
        int index = std::distance(ldtk::TileRenderMode_str.begin(), it);
        // set the enum value
        entitydef.tileRenderMode = static_cast<ldtk::TileRenderMode>(index);
        // tilesetId
        if (entity.find("tilesetId") != entity.end())
        {
            entitydef.tilesetId = entity["tilesetId"].get<int>();
        }
        else
        {
            entitydef.tilesetId = std::nullopt;
        }
        // uiTileRect
        if (entity.find("uiTileRect") != entity.end())
        {
            entitydef.uiTileRect = load_tileset_rect(entity["uiTileRect"]);
        }
        else
        {
            entitydef.uiTileRect = std::nullopt;
        }
        // uid
        entitydef.uid = entity["uid"].get<int>();
        // width
        entitydef.width = entity["width"].get<int>();
        // tileId
        if (entity.find("tileId") != entity.end())
        {
            entitydef.tileId = entity["tileId"].get<int>();
        }
        else
        {
            entitydef.tileId = std::nullopt;
        }
        entities.push_back(entitydef);
    }
    return entities;
}

ldtk::TilesetRect DataLoader::load_tileset_rect(nlohmann::json json)
{
    ldtk::TilesetRect tilerec;
    // h
    tilerec.h = json["h"].get<int>();
    // tileesetUid
    tilerec.tileesetUid = json["tileesetUid"].get<int>();
    // w
    tilerec.w = json["w"].get<int>();
    // x
    tilerec.x = json["x"].get<int>();
    // y
    tilerec.y = json["y"].get<int>();
    return tilerec;
}

std::vector<ldtk::Enum> DataLoader::load_enum(nlohmann::json json)
{
    std::vector<ldtk::Enum> enums;
    for (auto &value : json)
    {
        ldtk::Enum tmpenum;
        // externalRelPath
        if (value.find("externalRelPath") != value.end())
        {
            tmpenum.externalRelPath = value["externalRelPath"].get<std::string>();
        }
        else
        {
            tmpenum.externalRelPath = std::nullopt;
        }
        // iconTilesetUid
        if (value.find("iconTilesetUid") != value.end())
        {
            tmpenum.iconTilesetUid = value["iconTilesetUid"].get<int>();
        }
        else
        {
            tmpenum.iconTilesetUid = std::nullopt;
        }
        // identifier
        tmpenum.identifier = value["identifier"].get<std::string>();
        // tags
        for (auto &tag : value["tags"])
        {
            tmpenum.tags.push_back(tag.get<std::string>());
        }
        // uid
        tmpenum.uid = value["uid"].get<int>();
        // values
        tmpenum.values = load_enum_value(value["values"]);

        enums.push_back(tmpenum);
    }

    return enums;
}

std::vector<ldtk::EnumValue> DataLoader::load_enum_value(nlohmann::json json)
{
    std::vector<ldtk::EnumValue> enumvalues;
    for (auto &value : json)
    {
        ldtk::EnumValue tmpenumvalue;
        // color
        tmpenumvalue.color = value["color"].get<int>();
        // id
        tmpenumvalue.id = value["id"].get<std::string>();
        // tileRect
        if (value.find("tileRect") != value.end())
        {
            tmpenumvalue.tileRect = load_tileset_rect(value["tileRect"]);
        }
        else
        {
            tmpenumvalue.tileRect = std::nullopt;
        }
        // tileId
        if (value.find("tileId") != value.end())
        {
            tmpenumvalue.tileId = value["tileId"].get<int>();
        }
        else
        {
            tmpenumvalue.tileId = std::nullopt;
        }
        // __tileSrcRect (array of int)
        if (value.find("__tileSrcRect") != value.end())
        {
            std::vector<int> tmp;
            for (auto &tileSrcRect : value["__tileSrcRect"])
            {
                tmp.push_back(tileSrcRect.get<int>());
            }
            tmpenumvalue.__tileSrcRect = tmp;
        }
        else
        {
            tmpenumvalue.__tileSrcRect = std::nullopt;
        }
    }
    return enumvalues;
}

std::vector<ldtk::layerdef> DataLoader::load_layerdefs(nlohmann::json json)
{
    std::vector<ldtk::layerdef> layerdefs;
    for (auto &value : json)
    {
        ldtk::layerdef templd;
        // __type
        templd.__type = value["__type"].get<std::string>();
        // autoSourceLayerDefUid
        if (value.find("autoSourceLayerDefUid") != value.end())
        {
            templd.autoSourceLayerDefUid = value["autoSourceLayerDefUid"].get<int>();
        }
        else
        {
            templd.autoSourceLayerDefUid = std::nullopt;
        }
        // displayOpacity
        templd.displayOpacity = value["displayOpacity"].get<float>();
        // gridSize
        templd.gridSize = value["gridSize"].get<int>();
        // identifier
        templd.identifier = value["identifier"].get<std::string>();
        // intGridValues
        {
            ldtk::IntGridValues tmp;
            // color
            tmp.color = value["intGridValues"]["color"].get<std::string>();
            // groupUid
            tmp.groupUid = value["intGridValues"]["groupUid"].get<int>();
            // identifier
            if (value["intGridValues"].find("identifier") != value["intGridValues"].end())
            {
                tmp.identifier = value["intGridValues"]["identifier"].get<std::string>();
            }
            else
            {
                tmp.identifier = std::nullopt;
            }
            // tile
            if (value["intGridValues"].find("tile") != value["intGridValues"].end())
            {
                tmp.tile = load_tileset_rect(value["intGridValues"]["tile"]);
            }
            else
            {
                tmp.tile = std::nullopt;
            }
            // value
            tmp.value = value["intGridValues"]["value"].get<int>();
            templd.intGridValues = tmp;
        }
        // intGridValueGroups
        {
            ldtk::IntGridValueGroups tmp;
            // color
            if (value["intGridValueGroups"].find("color") != value["intGridValueGroups"].end())
            {
                tmp.color = value["intGridValueGroups"]["color"].get<std::string>();
            }
            else
            {
                tmp.color = std::nullopt;
            }
            // identifier
            if (value["intGridValueGroups"].find("identifier") != value["intGridValueGroups"].end())
            {
                tmp.identifier = value["intGridValueGroups"]["identifier"].get<std::string>();
            }
            else
            {
                tmp.identifier = std::nullopt;
            }
            // uid
            tmp.uid = value["intGridValueGroups"]["uid"].get<int>();
            templd.intGridValueGroups = tmp;
        }
        // parallaxFactorX
        templd.parallaxFactorX = value["parallaxFactorX"].get<float>();
        // parallaxFactorY
        templd.parallaxFactorY = value["parallaxFactorY"].get<float>();
        // parallaxScaling
        templd.parallaxScaling = value["parallaxScaling"].get<bool>();
        // pxOffsetX
        templd.pxOffsetX = value["pxOffsetX"].get<int>();
        // pxOffsetY
        templd.pxOffsetY = value["pxOffsetY"].get<int>();
        // tilesetDefUid
        if (value.find("tilesetDefUid") != value.end())
        {
            templd.tilesetDefUid = value["tilesetDefUid"].get<int>();
        }
        else
        {
            templd.tilesetDefUid = std::nullopt;
        }
        // uid
        templd.uid = value["uid"].get<int>();
        // autoTilesetDefUid
        if (value.find("autoTilesetDefUid") != value.end())
        {
            templd.autoTilesetDefUid = value["autoTilesetDefUid"].get<int>();
        }
        else
        {
            templd.autoTilesetDefUid = std::nullopt;
        }
    }
    return layerdefs;
}

std::vector<ldtk::Tileset> DataLoader::load_tilesets(nlohmann::json json)
{
    std::vector<ldtk::Tileset> tilesets;
    for (auto &value : json)
    {
        ldtk::Tileset tmptileset;
        // __cHei
        tmptileset.__cHei = value["__cHei"].get<int>();
        // __cWid
        tmptileset.__cWid = value["__cWid"].get<int>();
        // customData
        for (auto &customData : value["customData"])
        {
            ldtk::TilesetCustomData tmpcustomdata;
            // data
            tmpcustomdata.data = customData["data"].get<std::string>();
            // tileId
            tmpcustomdata.tileId = customData["tileId"].get<int>();
            tmptileset.customData.push_back(tmpcustomdata);
        }
        // enumTags
        for (auto &enumTags : value["enumTags"])
        {
            ldtk::TilesetEnumTags tmpenumtags;
            // enumValueId
            tmpenumtags.enumValueId = enumTags["enumValueId"].get<std::string>();
            // tileIds
            for (auto &tileIds : enumTags["tileIds"])
            {
                tmpenumtags.tileIds.push_back(tileIds.get<int>());
            }
            tmptileset.enumTags.push_back(tmpenumtags);
        }
        // identifier
        tmptileset.identifier = value["identifier"].get<std::string>();
        // padding
        tmptileset.padding = value["padding"].get<int>();
        // pxHei
        tmptileset.pxHei = value["pxHei"].get<int>();
        // pxWid
        tmptileset.pxWid = value["pxWid"].get<int>();
        // relPath
        if (value.find("relPath") != value.end())
        {
            tmptileset.relPath = value["relPath"].get<std::string>();
        }
        else
        {
            tmptileset.relPath = std::nullopt;
        }
        // spacing
        tmptileset.spacing = value["spacing"].get<int>();
        // tags
        for (auto &tag : value["tags"])
        {
            tmptileset.tags.push_back(tag.get<std::string>());
        }
        // tagsSourceTilesetUid
        if (value.find("tagsSourceTilesetUid") != value.end())
        {
            tmptileset.tagsSourceTilesetUid = value["tagsSourceTilesetUid"].get<int>();
        }
        else
        {
            tmptileset.tagsSourceTilesetUid = std::nullopt;
        }
        // tileGridSize
        tmptileset.tileGridSize = value["tileGridSize"].get<int>();
        // uid
        tmptileset.uid = value["uid"].get<int>();

        tilesets.push_back(tmptileset);
    }
    return tilesets;
}

std::vector<ldtk::Level> DataLoader::load_levels(nlohmann::json json)
{
    std::vector<ldtk::Level> levels;
    for (auto &value : json)
    {
        ldtk::Level tmplevel;
        // bgColor
        tmplevel.bgColor = value["bgColor"].get<std::string>();
        // __bgPos
        if (value.find("__bgPos") != value.end())
        {
            // cropRect
            for (int i = 0; i < 4; i++)
            {
                tmplevel.__bgPos.value().cropRect[i] = value["__bgPos"]["cropRect"][i].get<int>();
            }
            // scale
            for (int i = 0; i < 2; i++)
            {
                tmplevel.__bgPos.value().scale[i] = value["__bgPos"]["scale"][i].get<int>();
            }
            // topLeftPx
            for (int i = 0; i < 2; i++)
            {
                tmplevel.__bgPos.value().topLeftPx[i] = value["__bgPos"]["topLeftPx"][i].get<int>();
            }
        }
        else
        {
            tmplevel.__bgPos = std::nullopt;
        }
        // __neighbours
        for (auto &neighbours : value["__neighbours"])
        {
            ldtk::Neighbour tmpneighbour;
            // dir
            tmpneighbour.dir = neighbours["dir"].get<std::string>();
            // levelIid
            tmpneighbour.levelIid = neighbours["levelIid"].get<std::string>();
            // levelUid
            if (neighbours.find("levelUid") != neighbours.end())
            {
                tmpneighbour.levelUid = neighbours["levelUid"].get<int>();
            }
            else
            {
                tmpneighbour.levelUid = std::nullopt;
            }
            tmplevel.__neighbours.push_back(tmpneighbour);
        }
        // bgRelPath
        if (value.find("bgRelPath") != value.end())
        {
            tmplevel.bgRelPath = value["bgRelPath"].get<std::string>();
        }
        else
        {
            tmplevel.bgRelPath = std::nullopt;
        }
        // externalRelPath
        if (value.find("externalRelPath") != value.end())
        {
            tmplevel.externalRelPath = value["externalRelPath"].get<std::string>();
        }
        else
        {
            tmplevel.externalRelPath = std::nullopt;
        }
        // fieldInstances
        tmplevel.fieldInstances = load_field_instances(value["fieldInstances"]);
        // identifier
        tmplevel.identifier = value["identifier"].get<std::string>();
        // iid
        tmplevel.iid = value["iid"].get<std::string>();
        // layerInstances
        tmplevel.layerInstances = load_layer_instances(value["layerInstances"]);
        // pxHei
        tmplevel.pxHei = value["pxHei"].get<int>();
        // pxWid
        tmplevel.pxWid = value["pxWid"].get<int>();
        // uid
        tmplevel.uid = value["uid"].get<int>();
        // worldDepth
        tmplevel.worldDepth = value["worldDepth"].get<int>();
        // worldX
        tmplevel.worldX = value["worldX"].get<int>();
        // worldY
        tmplevel.worldY = value["worldY"].get<int>();

        levels.push_back(tmplevel);
    }
    return levels;
}

std::vector<ldtk::FieldInstance> DataLoader::load_field_instances(nlohmann::json json)
{
    // WIP
    std::vector<ldtk::FieldInstance> fieldinstances;
    for (auto &fieldInstance : json)
    {
        ldtk::FieldInstance tmpfieldinstance;
        // __identifier
        tmpfieldinstance.__identifier = fieldInstance["__identifier"].get<std::string>();
        // __tile
        tmpfieldinstance.__tile = load_tileset_rect(fieldInstance["__tile"]);
        // __type
        tmpfieldinstance.__type = fieldInstance["__type"].get<std::string>();
        // value
        {
            // tmpfieldinstance.__type define the type
            if (tmpfieldinstance.__type == "Integer")
            {
                tmpfieldinstance.value = fieldInstance["value"].get<int>();
            }
            else if (tmpfieldinstance.__type == "Float")
            {
                tmpfieldinstance.value = fieldInstance["value"].get<float>();
            }
            else if (tmpfieldinstance.__type == "Boolean")
            {
                tmpfieldinstance.value = fieldInstance["value"].get<bool>();
            }
            else if (tmpfieldinstance.__type == "String" || tmpfieldinstance.__type == "Text" || tmpfieldinstance.__type == "Multilines" || tmpfieldinstance.__type == "FilePath" || tmpfieldinstance.__type == "Color")
            {
                tmpfieldinstance.value = fieldInstance["value"].get<std::string>();
            }
            else if (tmpfieldinstance.__type.find("Enum") != std::string::npos)
            {
                tmpfieldinstance.value = fieldInstance["value"].get<std::string>();
            }
            else if (tmpfieldinstance.__type == "Point")
            {
                int tmp[2];
                for (int i = 0; i < 2; i++)
                {
                    tmp[i] = fieldInstance["value"][i].get<int>();
                }
                tmpfieldinstance.value = tmp;
            }
            else if (tmpfieldinstance.__type == "Tile")
            {
                tmpfieldinstance.value = load_tileset_rect(fieldInstance["value"]);
            }
            else if (tmpfieldinstance.__type == "EntityRef")
            {
                ldtk::EntityRef tmp;
                // entityIid
                tmp.entityIid = fieldInstance["value"]["entityIid"].get<std::string>();
                // layerIid
                tmp.layerIid = fieldInstance["value"]["layerIid"].get<std::string>();
                // levelIid
                tmp.levelIid = fieldInstance["value"]["levelIid"].get<std::string>();
                // worldIid
                tmp.worldIid = fieldInstance["value"]["worldIid"].get<std::string>();
                tmpfieldinstance.value = tmp;
            }
        }

        // defUid
        tmpfieldinstance.defUid = fieldInstance["defUid"].get<int>();

        fieldinstances.push_back(tmpfieldinstance);
    }
    return fieldinstances;
}

std::vector<ldtk::LayerInstance> DataLoader::load_layer_instances(nlohmann::json json)
{
    std::vector<ldtk::LayerInstance> layerinstances;
    for (auto &fieldInstance : json)
    {
        ldtk::LayerInstance tmpld;
        // __cHei
        tmpld.__cHei = fieldInstance["__cHei"].get<int>();
        // __cWid
        tmpld.__cWid = fieldInstance["__cWid"].get<int>();
        // __gridSize
        tmpld.__gridSize = fieldInstance["__gridSize"].get<int>();
        // __identifier
        tmpld.__identifier = fieldInstance["__identifier"].get<std::string>();
        // opacity
        tmpld.__opacity = fieldInstance["__opacity"].get<float>();
        // __pxTotalOffsetX
        tmpld.__pxTotalOffsetX = fieldInstance["__pxTotalOffsetX"].get<int>();
        // __pxTotalOffsetY
        tmpld.__pxTotalOffsetY = fieldInstance["__pxTotalOffsetY"].get<int>();
        // __tilesetDefUid
        if (fieldInstance.find("__tilesetDefUid") != fieldInstance.end())
        {
            tmpld.__tilesetDefUid = fieldInstance["__tilesetDefUid"].get<int>();
        }
        else
        {
            tmpld.__tilesetDefUid = std::nullopt;
        }
        // __tilesetRelPath
        if (fieldInstance.find("__tilesetRelPath") != fieldInstance.end())
        {
            tmpld.__tilesetRelPath = fieldInstance["__tilesetRelPath"].get<std::string>();
        }
        else
        {
            tmpld.__tilesetRelPath = std::nullopt;
        }
        // __type
        tmpld.__type = fieldInstance["__type"].get<std::string>();
        // autoLayerTiles
        tmpld.autoLayerTiles = load_tile_instances(fieldInstance["autoLayerTiles"]);
        // entityInstances
        tmpld.entityInstances = load_entity_instances(fieldInstance["entityInstances"]);
        // gridTiles
        tmpld.gridTiles = load_tile_instances(fieldInstance["gridTiles"]);
        // iid
        tmpld.iid = fieldInstance["iid"].get<std::string>();
        // intGridCsv
        for (auto &intGridCsv : fieldInstance["intGridCsv"])
        {
            tmpld.intGridCsv.push_back(intGridCsv.get<int>());
        }
        // layerDefUid
        tmpld.layerDefUid = fieldInstance["layerDefUid"].get<int>();
        // levelId
        tmpld.levelId = fieldInstance["levelId"].get<int>();
        // overrideTilesetUid
        if (fieldInstance.find("overrideTilesetUid") != fieldInstance.end())
        {
            tmpld.overrideTilesetUid = fieldInstance["overrideTilesetUid"].get<int>();
        }
        else
        {
            tmpld.overrideTilesetUid = std::nullopt;
        }
        // pxOffsetX
        tmpld.pxOffsetX = fieldInstance["pxOffsetX"].get<int>();
        // pxOffsetY
        tmpld.pxOffsetY = fieldInstance["pxOffsetY"].get<int>();
        // visible
        tmpld.visible = fieldInstance["visible"].get<bool>();
        // intGrid
        if (fieldInstance.find("intGrid") != fieldInstance.end())
        {
            std::vector<int> tmp;
            for (auto &intGrid : fieldInstance["intGrid"])
            {
                tmp.push_back(intGrid.get<int>());
            }
            tmpld.intGrid = tmp;
        }
        else
        {
            tmpld.intGrid = std::nullopt;
        }

        layerinstances.push_back(tmpld);
    }
    return layerinstances;
}

std::vector<ldtk::TileInstance> DataLoader::load_tile_instances(nlohmann::json json)
{
    std::vector<ldtk::TileInstance> tileinstances;
    for (auto &fieldInstance : json)
    {
        ldtk::TileInstance tmptile;
        // a
        tmptile.a = fieldInstance["a"].get<float>();
        // f
        tmptile.f = fieldInstance["f"].get<int>();
        // px
        for (int i = 0; i < 2; i++)
        {
            tmptile.px[i] = fieldInstance["px"][i].get<int>();
        }
        // src
        for (int i = 0; i < 2; i++)
        {
            tmptile.src[i] = fieldInstance["src"][i].get<int>();
        }
        // t
        tmptile.t = fieldInstance["t"].get<int>();

        tileinstances.push_back(tmptile);
    }

    return tileinstances;
}

std::vector<ldtk::EntityInstance> DataLoader::load_entity_instances(nlohmann::json json)
{
    std::vector<ldtk::EntityInstance> entityinstances;
    for (auto &fieldInstance : json)
    {
        ldtk::EntityInstance tmpentity;
        // __grid
        for (int i = 0; i < 2; i++)
        {
            tmpentity.__grid[i] = fieldInstance["__grid"][i].get<int>();
        }
        // __identifier
        tmpentity.__identifier = fieldInstance["__identifier"].get<std::string>();
        // __pivot
        for (int i = 0; i < 2; i++)
        {
            tmpentity.__pivot[i] = fieldInstance["__pivot"][i].get<float>();
        }
        // __smartColor
        tmpentity.__smartColor = fieldInstance["__smartColor"].get<std::string>();
        // __tags
        for (auto &tag : fieldInstance["__tags"])
        {
            tmpentity.__tags.push_back(tag.get<std::string>());
        }
        // __tile
        if (fieldInstance.find("__tile") != fieldInstance.end())
        {
            tmpentity.__tile = load_tileset_rect(fieldInstance["__tile"]);
        }
        else
        {
            tmpentity.__tile = std::nullopt;
        }
        // __worldX
        if (fieldInstance.find("__worldX") != fieldInstance.end())
        {
            tmpentity.__worldX = fieldInstance["__worldX"].get<int>();
        }
        else
        {
            tmpentity.__worldX = std::nullopt;
        }
        // __worldY
        if (fieldInstance.find("__worldY") != fieldInstance.end())
        {
            tmpentity.__worldY = fieldInstance["__worldY"].get<int>();
        }
        else
        {
            tmpentity.__worldY = std::nullopt;
        }
        // defUid
        tmpentity.defUid = fieldInstance["defUid"].get<int>();
        // fieldInstances
        tmpentity.fieldInstances = load_field_instances(fieldInstance["fieldInstances"]);
        // height
        tmpentity.height = fieldInstance["height"].get<int>();
        // iid
        tmpentity.iid = fieldInstance["iid"].get<std::string>();
        // px
        for (int i = 0; i < 2; i++)
        {
            tmpentity.px[i] = fieldInstance["px"][i].get<int>();
        }
        // width
        tmpentity.width = fieldInstance["width"].get<int>();

        entityinstances.push_back(tmpentity);
    }
    return entityinstances;
}
