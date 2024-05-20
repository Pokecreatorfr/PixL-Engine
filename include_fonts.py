import os
import re

def parse_config_file(file_path):
    # Modèle d'expression régulière pour rechercher les directives #define avec des listes
    define_pattern = re.compile(r'const\s+int\s+(\w+)\s*\[\s*\]\s*=\s*\{([^}]+)\}', re.MULTILINE)

    # Dictionnaire pour stocker les valeurs des #define
    define_values = {}

    with open(file_path, 'r') as file:
        content = file.read()

        # Recherche des correspondances dans le contenu du fichier
        matches = define_pattern.findall(content)

        # Stockage des résultats dans le dictionnaire
        for match in matches:
            define_name, define_list = match

            # Si c'est une liste, séparez les valeurs et convertissez-les en entiers
            values = [int(x.strip()) for x in define_list.split(',')]
            define_values[define_name] = values

    return define_values
config_file_path = 'include/const/Config.hpp'
result = parse_config_file(config_file_path)

if isinstance(result['fonts_sizes'], int):
    result['fonts_sizes'] = [result['fonts_sizes']]

def generate_asm_image_entry(image_filename):
    base_name = os.path.splitext(os.path.basename(image_filename))[0]
    asm_entry = f'.global font_{base_name}\n.align 4\n\nfont_{base_name}:\n.incbin "{image_filename}"\n\n'
    asm_entry += f'.global font_{base_name}_size\n.align 4\n\nfont_{base_name}_size:\n.int 0x00000000 + font_{base_name}_size - font_{base_name}\n\n\n'
    return asm_entry

def generate_hpp_image_entry(image_filename):
    base_name = os.path.splitext(os.path.basename(image_filename))[0]
    hpp_entry = f'extern const char font_{base_name}[];\nextern const int font_{base_name}_size;\n'
    return hpp_entry

def generate_hpp_resource_entry(image_filename):
    base_name = os.path.splitext(os.path.basename(image_filename))[0]
    resource_entry = f'const image_ressource font_{base_name}_ressource = {{ font_{base_name}, &font_{base_name}_size }};\n'
    return resource_entry


image_files = [f for f in os.listdir('graphics/fonts') if f.lower().endswith(('.png', '.jpg', '.bmp'))]

with open('asm/graphic.s', 'a') as asm_file:

    for image in image_files:
        asm_entry = ".section rodata\n\n"
        asm_entry = generate_asm_image_entry(os.path.join('graphics/fonts', image).replace("\\","/"))
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


ttf_list =[]
font_hpp_file = '#pragma once\n#include <Fonts.hpp>\n#include <generated/image2cpp.hpp>\n\n'
for image in image_files:
    ttf_list.append(image.split('_')[1])

# delete duplicate
ttf_list = list(dict.fromkeys(ttf_list))

#delete extension
ttf_list = [os.path.splitext(x)[0] for x in ttf_list]

i = 0

for font in ttf_list:
    font_list = []
    for size in result['fonts_sizes']:
        font_list.append(f'font_{font}_{size}')
        font_hpp_file += f'const font font_{font}_{size} = {{{size} , &font_{size}_{font}_ressource }};\n'
    font_hpp_file += f'\nconst font_ressource font_ressource_{font} = {{{i},{{'
    for i in range(len(font_list)):
        font_hpp_file += f'&{font_list[i]}'
        if i != len(font_list) - 1:
            font_hpp_file += ', '
    font_hpp_file += '}};\n\n'

    

with open('include/generated/font2cpp.hpp', 'w') as hpp_file:
    hpp_file.write(font_hpp_file)