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
    generated_dir = 'include\generated'
    include_dir = 'include\generated\image2cpp.hpp'

    if not os.path.exists(asm_dir):
        os.makedirs(asm_dir)
        print(f'Created folder: {asm_dir}')

    if not os.path.exists(generated_dir):
        os.makedirs(generated_dir)
        print(f'Created folder: {generated_dir}')

    image_files = [f for f in os.listdir(image_dir) if f.lower().endswith(('.png', '.jpg', '.bmp'))]

    with open(os.path.join(asm_dir, 'graphic.s'), 'w') as asm_file:
        for image in image_files:
            asm_entry = ".section rodata\n\n"
            asm_entry += generate_asm_image_entry(os.path.join(image_dir, image).replace("\\","/"))
            asm_file.write(asm_entry)

    print(f'Processed {len(image_files)} images and wrote to graphic.s')

    with open(os.path.join(os.path.dirname(__file__), include_dir), 'w') as hpp_file:
        hpp_file.write('#pragma once\n')
        hpp_file.write('#include <Graphics.hpp>\n\n')
        
        for image in image_files:
            hpp_entry = generate_hpp_image_entry(image)
            hpp_file.write(hpp_entry)

        hpp_file.write('\n')
        
        for image in image_files:
            resource_entry = generate_hpp_resource_entry(image)
            hpp_file.write(resource_entry)
        print(f'Generated image2cpp.hpp')

generate_files()
