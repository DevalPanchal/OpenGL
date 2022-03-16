#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdio.h>
#include <iostream>
#include "Shaders.h"
#include "../tiny_obj_loader.h"
#include <vector>

class Model {
public:
	struct Mesh
	{
		GLuint vbuffer;		// vertex buffer name
		GLuint ibuffer;		// index buffer name
		GLuint triangles;	// number of triangles
		GLuint vbytes;		// size of vertices in bytes
		GLuint program;		// program for drawing the mesh
		glm::mat4 model;	// model transformation for the mesh
	};
	std::vector<Mesh*> Model;

	double theta, phi;
	double r;

	glm::mat4 projection;
	float eyex, eyey, eyez;

	Mesh* loadOBJ(char* filename) {
		GLuint vbuffer;
		GLuint ibuffer;
		GLuint objVAO;
		GLfloat* vertices;
		GLfloat* normals;
		GLuint* indices;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		int nv;
		int nn;
		int ni;
		int i;
		float xmin, ymin, zmin;
		float xmax, ymax, zmax;
		float dx, dy, dz;
		float scale;
		int triangles;
		Mesh* mesh;

		glGenVertexArrays(1, &objVAO);
		glBindVertexArray(objVAO);

		/*  Load the obj file */

		std::string err = tinyobj::LoadObj(shapes, materials, filename, 0);

		if (!err.empty()) {
			std::cerr << err << std::endl;
			return(NULL);
		}

		/*  Retrieve the vertex coordinate data */

		nv = shapes[0].mesh.positions.size();
		vertices = new GLfloat[nv];
		for (i = 0; i < nv; i++) {
			vertices[i] = shapes[0].mesh.positions[i];
		}

		/*
		 *  Find the range of the x, y and z
		 *  coordinates.
		 */

		xmin = ymin = zmin = 1000000.0;
		xmax = ymax = zmax = -1000000.0;
		for (i = 0; i < nv / 3; i++) {
			if (vertices[3 * i] < xmin)
				xmin = vertices[3 * i];
			if (vertices[3 * i] > xmax)
				xmax = vertices[3 * i];
			if (vertices[3 * i + 1] < ymin)
				ymin = vertices[3 * i + 1];
			if (vertices[3 * i + 1] > ymax)
				ymax = vertices[3 * i + 1];
			if (vertices[3 * i + 2] < zmin)
				zmin = vertices[3 * i + 2];
			if (vertices[3 * i + 2] > zmax)
				zmax = vertices[3 * i + 2];
		}

		/*
		 *  Scales the vertices so the longest axis is unit length
		*/

		dx = xmax - xmin;
		dy = ymax - ymin;
		dz = zmax - zmin;

		scale = dx;
		if (dy > scale)
			scale = dy;
		if (dz > scale)
			scale = dz;

		scale = 1.0 / scale;
		for (i = 0; i < nv; i++) {
			vertices[i] = vertices[i] * scale;
		}

		/*  Retrieve the vertex normals */

		nn = shapes[0].mesh.normals.size();
		normals = new GLfloat[nn];
		for (i = 0; i < nn; i++) {
			normals[i] = shapes[0].mesh.normals[i];
		}

		/*  Retrieve the triangle indices */

		ni = shapes[0].mesh.indices.size();
		triangles = ni / 3;
		indices = new GLuint[ni];
		for (i = 0; i < ni; i++) {
			indices[i] = shapes[0].mesh.indices[i];
		}

		/*
		 *  load the vertex coordinate data
		 */
		glGenBuffers(1, &vbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
		glBufferData(GL_ARRAY_BUFFER, (nv + nn) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, nv * sizeof(GLfloat), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, nv * sizeof(GLfloat), nn * sizeof(GLfloat), normals);

		/*
		 *  load the vertex indexes
		 */
		glGenBuffers(1, &ibuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ni * sizeof(GLuint), indices, GL_STATIC_DRAW);

		/*
		 *  Allocate the mesh structure and fill it in
		*/
		mesh = new Mesh();
		mesh->vbuffer = vbuffer;
		mesh->ibuffer = ibuffer;
		mesh->triangles = triangles;
		mesh->vbytes = nv * sizeof(GLfloat);

		delete[] vertices;
		delete[] normals;
		delete[] indices;

		return(mesh);
	}


	GLuint loadProgram(char* vertex, char* fragment) {
		GLuint vs;
		GLuint fs;
		GLuint program;

		/*
		 *  compile and build the shader program
		 */
		vs = buildShader(GL_VERTEX_SHADER, vertex);
		fs = buildShader(GL_FRAGMENT_SHADER, fragment);
		program = buildProgram(vs, fs, 0);

		return(program);
	}


	Mesh* copyObject(Mesh* obj) {
		Mesh* result;

		result = new Mesh();
		*result = *obj;
		return(result);
	}
};

#endif 