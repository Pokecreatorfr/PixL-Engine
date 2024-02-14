import os
import json

def generate_asm_image_entry(image_filename):
    base_name = os.path.splitext(os.path.basename(image_filename))[0]
    asm_entry = f'.global tileset_{base_name}\n.align 4\n\ntileset_{base_name}:\n.incbin "{image_filename}"\n\n'
    asm_entry += f'.global tileset_{base_name}_size\n.align 4\n\ntileset_{base_name}_size:\n.int 0x00000000 + tileset_{base_name}_size - tileset_{base_name}\n\n\n'
    return asm_entry

def generate_hpp_image_entry(image_filename):
    base_name = os.path.splitext(os.path.basename(image_filename))[0]
    hpp_entry = f'extern const char tileset_{base_name}[];\nextern const int tileset_{base_name}_size;\n'
    return hpp_entry

def generate_hpp_resource_entry(image_filename):
    base_name = os.path.splitext(os.path.basename(image_filename))[0]
    resource_entry = f'const image_ressource tileset_{base_name}_ressource = {{ tileset_{base_name}, &tileset_{base_name}_size }};\n'
    return resource_entry

# list all files in graphics/tilesets

image_files = [f for f in os.listdir('graphics/tilesets') if f.lower().endswith(('.png', '.jpg', '.bmp'))]


with open('asm/graphic.asm', 'a') as asm_file:

    

    for image in image_files:
        asm_entry = ".section rodata\n\n"
        asm_entry = generate_asm_image_entry(os.path.join('graphics/tilesets', image).replace("\\","/"))
        asm_file.write(asm_entry)

with open('include\generated\image2cpp.hpp', 'a') as hpp_file:

    hpp_file.write('\n\n')

    for image in image_files:
        hpp_entry = generate_hpp_image_entry(image)
        hpp_file.write(hpp_entry)

    hpp_file.write('\n')

    for image in image_files:
        resource_entry = generate_hpp_resource_entry(image)
        hpp_file.write(resource_entry)

# open json data/maps/world.ldtk
    
with open('data/maps/world.ldtk', 'r') as json_file:
    data = json.load(json_file)

    hpp_file = "#pragma once\n"
    hpp_file += "#include <Tileset.hpp>\n"
    hpp_file += "#include <generated/image2cpp.hpp>\n\n"



    for tilesets in data['defs']['tilesets']:
        image_filename = tilesets['relPath']
        base_name = os.path.splitext(os.path.basename(image_filename))[0]
        height = tilesets['__cHei']
        width = tilesets['__cWid']
        uid = tilesets['uid']

        hpp_file += f"const tileset Tileset_{base_name} =\n{{\n"
        hpp_file += f"    {uid},\n"
        hpp_file += f"    {width},\n"
        hpp_file += f"    {height},\n"
        hpp_file += f"    &tileset_{base_name}_ressource\n}};\n\n"
    

# write generated/tileset2cpp.hpp

with open(os.path.join(os.path.dirname(__file__), 'include/generated/tileset2cpp.hpp'), 'w') as file:
    file.write(hpp_file)
    print(f'Generated tileset2cpp.hpp')
