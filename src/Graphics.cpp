#include <Graphics.hpp>

OpenGLTexture::OpenGLTexture(const image_ressource* image_ressource)
{
	SDL_RWops* rw = SDL_RWFromConstMem(image_ressource->image_ressource, *image_ressource->image_size);
	if (!rw)
	{
		std::cout << "Failed to create rwops\n";
		return;
	}


	SDL_Surface* surface = IMG_Load_RW(rw, 1);
	if (!surface)
	{
		std::cout << "Failed to create surface\n";
		return;
	}

	int Mode = GL_RGB;
	switch (surface->format->BytesPerPixel)
        {
        case 4:
            if (surface->format->Rmask == 0x000000ff)
                Mode = GL_RGBA;
            else
                Mode = GL_BGRA;
            break;
        case 3:
            if (surface->format->Rmask == 0x000000ff)
                Mode = GL_RGB;
            else
                Mode = GL_BGR;
            break;
        default:
            std::cout << "Error, image is not truecolor." << std::endl;
            return;
        }

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, surface->w, surface->h, 0, Mode, GL_UNSIGNED_BYTE, surface->pixels);

	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// check if texture is correctly loaded
	if (texture == 0)
	{
		std::cout << "Failed to create texture\n";
		return;
	}

	width = surface->w;
	height = surface->h;

	SDL_FreeSurface(surface);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTexture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, texture);
}

OpenGLTexture::~OpenGLTexture()
{
	glDeleteTextures(1, &texture);
}

int OpenGLTexture::GetWidth()
{
	return width;
}

int OpenGLTexture::GetHeight()
{
	return height;
}

GLuint OpenGLTexture::GetTexture()
{
	return texture;
}

OpenGLShader::OpenGLShader(const Shader shader)
{
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	program = glCreateProgram();

	glShaderSource(vertex, 1, &shader.vertex_shader, NULL);
	glCompileShader(vertex);
	int success;
	char infoLog[512];
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glShaderSource(fragment, 1, &shader.fragment_shader, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

OpenGLShader::~OpenGLShader()
{
	glDeleteProgram(program);
}

void OpenGLShader::Use()
{
	glUseProgram(program);
}

GLuint OpenGLShader::GetProgram()
{
	return program;
}
