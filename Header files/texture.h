#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "../Header files/stb_image.h"


class Texture {
public:
	unsigned int identificator;

public:
	Texture();

	void Inizialize(std::string path);
	void Bind(int binding_type);
};


#endif // TEXTURE_H


