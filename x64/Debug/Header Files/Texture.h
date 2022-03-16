#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <GL/glew.h>
#include "stb_image.h"

class Texture {
	public:
		GLuint ID;
		GLenum type;
		Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

		void texUnit(GLuint shaderProgram, const char* uniform, GLuint unit);
		void Bind();
		void Unbind();
		void Delete();
};

#endif 