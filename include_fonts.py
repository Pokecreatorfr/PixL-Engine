import os

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

with open('asm/graphic.asm', 'a') as asm_file:

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