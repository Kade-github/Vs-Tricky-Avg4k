#include "stbi.h"
#include "Rendering.h"
//Constructors and destructor
		
Texture* Texture::createWithImage(std::string filePath)
{
	Texture* t = stbi_h::stbi_load_file(filePath);
	t->fromSTBI = true;
	if (t->pixels == NULL)
	{
		std::cout << "pixels are fucking null? WTF STBI " << filePath << std::endl;
		return t;
	}
	if (stbi_h::stbi_error())
	{
		return t;
	}
	return t;
}

Texture* Texture::createFromSurface(SDL_Surface* surf, bool free) {
	if (surf == NULL)
	{
		std::cout << "Texture*: NULL PTR FROM SURFACE. FUCKING APE" << std::endl;
		return Rendering::white;
	}
	int w = surf->w;
	int h = surf->h;
	unsigned char* c = (unsigned char*)surf->pixels;
	if (free)
		SDL_FreeSurface(surf);
	return new Texture(c, w, h);
}

Texture::Texture(unsigned char* data, const unsigned int width, const unsigned int height)
{
	//Create texture object and use given data

	glGenTextures(1, &id);
	SetData(data, width, height);
}
		
Texture::~Texture()
{
	//Delete texture object
	glDeleteTextures(1, &id);
	if (pixels != nullptr && fromSTBI && width > 0 && height > 0)
		stbi_h::stbi_free(pixels);
}
		
//Set texture data function
bool Texture::SetData(unsigned char* data, const unsigned int _width, const unsigned int _height)
{
	pixels = data;
	//Bind texture object
	width = _width;
	height = _height;
	glBindTexture(GL_TEXTURE_2D, id);
	glEnable(GL_TEXTURE_2D);
			
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	//Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			
	return false;
}