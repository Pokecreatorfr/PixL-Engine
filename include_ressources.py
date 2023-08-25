import os

def generate_asm_image_entry(image_filename):
    base_name = os.path.splitext(os.path.basename(image_filename))[0]
    asm_entry = f'.global {base_name}\n.align 4\n\n{base_name}:\n.incbin "{image_filename}"\n\n'
    asm_entry += f'.global {base_name}_size\n.align 4\n\n{base_name}_size:\n.int 0x00000000 + {base_name}_size - {base_name}\n\n\n'
    return asm_entry

def generate_hpp_image_entry(image_filename):
    base_name = os.path.splitext(os.path.basename(image_filename))[0]
    hpp_entry = f'extern const char {base_name}[];\nextern const int {base_name}_size;\n'
    return hpp_entry

def generate_hpp_resource_entry(image_filename):
    base_name = os.path.splitext(os.path.basename(image_filename))[0]
    resource_entry = f'const image_ressource {base_name}_ressource = {{ {base_name}, &{base_name}_size }};\n'
    return resource_entry

def generate_files():
    image_dir = 'graphics'
    asm_dir = 'asm'
    include_dir = 'include'

    image_files = [f for f in os.listdir(image_dir) if f.lower().endswith(('.png', '.jpg', '.bmp'))]

    with open(os.path.join(asm_dir, 'graphic.asm'), 'w') as asm_file:
        for image in image_files:
            asm_entry = ".section rodata\n\n"
            asm_entry += generate_asm_image_entry(os.path.join(image_dir, image).replace("\\","/"))
            asm_file.write(asm_entry)

    with open(os.path.join(include_dir, 'imagetocpp.hpp'), 'w') as hpp_file:
        hpp_file.write('#pragma once\n')
        hpp_file.write('#include <Graphics.hpp>\n\n')
        
        for image in image_files:
            hpp_entry = generate_hpp_image_entry(image)
            hpp_file.write(hpp_entry)

        hpp_file.write('\n')
        
        for image in image_files:
            resource_entry = generate_hpp_resource_entry(image)
            hpp_file.write(resource_entry)


generate_files()
