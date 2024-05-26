#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <const/Config.hpp>
#include <glad/glad.h>
#include <generated/shaders.hpp>
#include <glm/glm.hpp>



using namespace std;

struct image_ressource
{
	const char* image_ressource;
	const int* image_size;
};

struct colorRGB
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

SDL_Texture* Load_Texture(image_ressource image_ressource, SDL_Renderer* renderer);
SDL_Texture* Load_Texture(image_ressource image_ressource, SDL_Renderer* renderer);

class OpenGLTexture
{
	public:
		OpenGLTexture(image_ressource image_ressource);
		void Bind();
		~OpenGLTexture();
		int GetWidth();
		int GetHeight();
		GLuint GetTexture();
	private:
		GLuint texture;
		int width;
		int height;
};

class OpenGLShader
{
	public:
		OpenGLShader(const Shader shader);
		~OpenGLShader();
		void Use();
		GLuint GetProgram();
	private:
		GLuint program;
};