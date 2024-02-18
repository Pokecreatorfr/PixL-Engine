import os
from PIL import Image, ImageDraw, ImageFont

fonts_files = [f for f in os.listdir('data/fonts') if f.lower().endswith(('.ttf'))]
images_path = 'graphics/fonts'
size = 16   # 16x16
char_per_line = 255
char_per_row = 255
for font in fonts_files:
    font_path = os.path.join('data/fonts', font).replace("\\","/")
    font_file = ImageFont.truetype(font_path, size)
    image = Image.new('RGBA', (size * char_per_line, size * char_per_row), color=(0, 0, 0, 0))
    draw = ImageDraw.Draw(image)

    x,y = 0,0

    for i in range(char_per_line * char_per_row):
        character = chr(i)
        draw.text((x, y), character, font=font_file, fill=(0, 0, 0, 255))

        x += size
        if x >= size * char_per_line:
            x = 0
            y += size
    
    output_image = os.path.join(images_path, os.path.splitext(font)[0] + '.png').replace("\\","/")
    image.save(output_image)


# create charmap in include/generated/charmap.hpp
charmap = ""
for i in range(char_per_line * char_per_row):
    charmap += "{{" + chr(i) + ',' + str(i) + "}},\n"

with open('include/generated/charmap.hpp', 'w') as file:
    file.write(f'#pragma once\n\n')
    file.write(f'const map<int,int> = {{\n')
    file.write(charmap)
    file.write(f'}};')
