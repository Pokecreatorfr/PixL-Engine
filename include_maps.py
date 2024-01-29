# read json files

import json
import os
import sys

# read json int data/maps/world.ldtk
path = os.path.join(os.path.dirname(__file__), 'data/maps/world.ldtk')
with open(path, 'r') as f:
    world = json.load(f)

cpp_file = "#pragma once\n\n#include <map_struct.hpp>\n\n"
cpp_file += f"#include <generated/tileset2cpp.hpp> \n"
cpp_file += f"#include <vector>\n\n"

maps_vector = "const std::vector<const map_struct*> overworld_maps = {\n"


for map in world['levels']:
    name = map['identifier']
    worldX = map['worldX'] // 32
    worldY = map['worldY'] // 32
    height = map['pxHei'] // 32
    width = map['pxWid'] // 32
    tile_layer0 = map['layerInstances'][4]['gridTiles']
    tile_layer1 = map['layerInstances'][3]['gridTiles']
    tile_layer2 = map['layerInstances'][2]['gridTiles']
    collision_layer = map['layerInstances'][1]['intGridCsv']
    tileset_uid = map['layerInstances'][4]['__tilesetDefUid']
    tileset_name = ""

    maps_vector += f"    &{name},\n"

    for tilesets in world['defs']['tilesets']:
        if tilesets['uid'] == tileset_uid:
            image_filename = tilesets['relPath']
            base_name = os.path.splitext(os.path.basename(image_filename))[0]
            tileset_name = "Tileset_"+base_name

    tile_layer0_array = [-1] * (height * width)

    for i in range(height * width):
        for tile in tile_layer0:
            #print(tile['px'], [i % width * 32, i // height * 32])
            if tile['px'] == [i % width * 32, i // width * 32]:
                tile_layer0_array[i] = tile['t']
                break
    
    tile_layer1_array = [-1] * (height * width)

    for i in range(height * width):
        for tile in tile_layer1:
            #print(tile['px'], [i % width * 32, i // height * 32])
            if tile['px'] == [i % width * 32, i // width * 32]:
                tile_layer1_array[i] = tile['t']
                break
    
    tile_layer2_array = [-1] * (height * width)

    for i in range(height * width):
        for tile in tile_layer2:
            #print(tile['px'], [i % width * 32, i // height * 32])
            if tile['px'] == [i % width * 32, i // width * 32]:
                tile_layer2_array[i] = tile['t']
                break
    
    
    cpp_file += f"const map_struct {name} = {{\n"
    cpp_file += f"    {width},\n    {height},\n"
    cpp_file += f"    {worldX},\n    {worldY},\n"
    cpp_file += f"    {{"
    for i in range(height * width):
        cpp_file += f"{tile_layer0_array[i]} "
        if(i < height * width - 1):
            cpp_file += ", "
    
    cpp_file += f"}},\n    {{"
    for i in range(height * width):
        cpp_file += f"{tile_layer1_array[i]} "
        if(i < height * width - 1):
            cpp_file += ", "
    
    cpp_file += f"}},\n    {{"
    for i in range(height * width):
        cpp_file += f"{tile_layer2_array[i]} "
        if(i < height * width - 1):
            cpp_file += ", "
    
    cpp_file += f"}}, \n    {{}},\n    {{"

    for i in range(len(collision_layer)):
        cpp_file += f"{collision_layer[i]} "
        if(i < height * width - 1):
            cpp_file += ", "
    
    cpp_file += f" }},\n"
    cpp_file += f"    &{tileset_name}\n}};\n\n"

maps_vector += "};\n\n"

cpp_file += maps_vector

# write to include/map2cpp.hpp
path = os.path.join(os.path.dirname(__file__), 'include/generated/map2cpp.hpp')
with open(path, 'w') as f:
    f.write(cpp_file)