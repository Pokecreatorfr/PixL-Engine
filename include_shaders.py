import os

def include_shaders(vert, frag, name):
    path = os.path.join(os.path.dirname(__file__), 'include/generated/shaders.hpp')
    with open(path, 'a') as f:
        f.write(f"const Shader Shader_{name} = {{\n")
        f.write(f"    \"{vert}\",\n")
        f.write(f"    \"{frag}\"\n")
        f.write("};\n\n")
# if generated/shaders.hpp exists, delete it
path = os.path.join(os.path.dirname(__file__), 'include/generated/shaders.hpp')

if os.path.exists(path):
    os.remove(path)

# write to include/generated/shaders.hpp
# struct Shader
#{
#    const char* vertex_shader;
#    const char* fragment_shader;
#};


with open(path , 'w') as f:
    f.write("#pragma once\n\n")
    f.write("struct Shader\n")
    f.write("{\n")
    f.write("    const char* vertex_shader;\n")
    f.write("    const char* fragment_shader;\n")
    f.write("};\n\n")

# for each directory in data/shaders compile the shaders into a struct shader.vert and shader.frag and write to include/generated/shaders.hpp , struct name is Shader + directory name , use inline strings with \n for newlines


path = os.path.join(os.path.dirname(__file__), 'data/shaders')

compiled_shaders = 0

for root, dirs, files in os.walk(path):
    for dir in dirs:
        # if folder does not contain shader.vert or shader.frag print error
        if not os.path.exists(os.path.join(path, dir, 'shader.vert')):
            print(f"Error: {os.path.join(path, dir)} does not contain shader.vert")
        elif not os.path.exists(os.path.join(path, dir, 'shader.frag')):
            print(f"Error: {os.path.join(path, dir)} does not contain shader.frag")
        else:
            print(f"Include shader: {os.path.join(path, dir)}")
            vert = ""
            frag = ""
            vert_path = os.path.join(path, dir, 'shader.vert')
            frag_path = os.path.join(path, dir, 'shader.frag')
            with open(vert_path, 'r') as f:
                vert = f.read()
            with open(frag_path, 'r') as f:
                frag = f.read()
            vert = vert.replace("\n", "\\n")
            frag = frag.replace("\n", "\\n")
            include_shaders(vert, frag, dir)
            compiled_shaders += 1

if compiled_shaders > 0:
    if compiled_shaders == 1:
        print(f"{compiled_shaders} shader is included in include/generated/shaders.hpp")
    else:    
        print(f"{compiled_shaders} shaders are included in include/generated/shaders.hpp")