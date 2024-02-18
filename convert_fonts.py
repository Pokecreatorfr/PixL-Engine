import os
from PIL import Image, ImageDraw, ImageFont
import re
def parse_config_file(file_path):
    # Modèle d'expression régulière pour rechercher les directives #define avec des listes
    define_pattern = re.compile(r'#define\s+(\w+)\s+(?:\{([^}]+)\}|(\d+))', re.MULTILINE)

    # Dictionnaire pour stocker les valeurs des #define
    define_values = {}

    with open(file_path, 'r') as file:
        content = file.read()

        # Recherche des correspondances dans le contenu du fichier
        matches = define_pattern.findall(content)

        # Stockage des résultats dans le dictionnaire
        for match in matches:
            define_name, define_list, define_value = match

            if define_list:
                # Si c'est une liste, séparez les valeurs et convertissez-les en entiers
                values = [int(x.strip()) for x in define_list.split(',')]
                define_values[define_name] = values
            else:
                define_values[define_name] = int(define_value)

    return define_values
# Remplacez 'chemin/vers/votre/config.hpp' par le chemin réel de votre fichier
config_file_path = 'include/const/Config.hpp'
result = parse_config_file(config_file_path)

fonts_files = [f for f in os.listdir('data/fonts') if f.lower().endswith(('.ttf'))]
images_path = 'graphics/fonts'
if not os.path.exists(images_path):
    os.makedirs(images_path)
    print(f'Created folder: {images_path}')
for file in os.listdir(images_path):
    os.remove(os.path.join(images_path, file))
char_per_line = 255
char_per_row = 4
for font in fonts_files:
    for size in result['FONTS_SIZES']:
        font_path = os.path.join('data/fonts', font).replace("\\","/")
        font_file = ImageFont.truetype(font_path, size)
        image = Image.new('RGBA', (size * char_per_line, size * char_per_row), color=(0, 0, 0, 0))
        draw = ImageDraw.Draw(image)

        x,y = 0,0

        for i in range(char_per_line * char_per_row):
            character = chr(i)
            draw.text((x, y), character, font=font_file, fill=(255, 255, 255, 255))

            x += size
            if x >= size * char_per_line:
                x = 0
                y += size

        output_image = os.path.join(images_path, str(size)+ '_' + os.path.splitext(font)[0] + '.png').replace("\\","/")
        image.save(output_image)
