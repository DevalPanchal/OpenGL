/************************************************
 *
 *             Example Six
 *
 *  An example showing how tiny obj loader can
 *  be used to load an obj file.  In this case
 * the object is the Blender Monkey
 *
 ************************************************/
#include "tiny_obj_loader.h" 
#include <iostream> 
#include <Windows.h>
#include <algorithm>
#include <unordered_set>

#include <gl/glew.h>
#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Header Files/Shaders.h"
#include "Header Files/VAO.h"
#include "Header Files/VBO.h"
#include "Header Files/EBO.h"

#include "Header Files/Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Header Files/stb_image.h"
// #include "camera.h"
#include "Header Files/Camera.h"
#include <random>
#include <map>
#include <stdio.h>
#include <set>


void shaderInit();
 /*
  * The following structure represents a single mesh in the scene
 */
struct Mesh {
	GLuint vbuffer;		// vertex buffer name
	GLuint ibuffer;		// index buffer name
	GLuint triangles;	// number of triangles
	GLuint vbytes;		// size of vertices in bytes
	GLuint program;		// program for drawing the mesh
	GLuint program2;
	glm::mat4 model;	// model transformation for the mesh
};


GLuint shaderProgram;
std::vector<Mesh*> Model;	// list of meshes in the scene
std::vector<Mesh*> GroundModel;

std::vector<VAO> modelVAO;

GLuint planeShaderProgram;
GLuint pyramidShaderProgram;

double theta, phi;
double r;

float cx, cy, cz;

glm::mat4 projection;	// projection matrix
float eyex, eyey, eyez;	// eye position

//Camera camera(glm::vec3(0.0f, 0.05f, 1.0f)); // camera

const unsigned int SCREEN_WIDTH = 1920;
const unsigned int SCREEN_HEIGHT = 1080;

Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float rotation;
float pyramidFloat;


GLfloat planeVertices[] =
{ //     COORDINATES     /        COLORS       //
	-0.5f, -0.5f, 0.0f,     0.0, 1.0f, 0.0f, // Lower left corner
	-0.5f,  0.5f, 0.0f,     0.0, 1.0f, 0.0f, // Upper left corner
	 0.5f,  0.5f, 0.0f,     0.0, 1.0f, 0.0f, // Upper right corner
	 0.5f, -0.5f, 0.0f,     0.0, 1.0f, 0.0f, // Lower right corner
};

// pyramid vertices
GLfloat vertices[] = {
	// COORDINATES			// COLORS				
	-0.5f, 0.0f,  0.5f,     0.329412, 0.329412, 0.329412,
	-0.5f, 0.0f, -0.5f,     0.329412, 0.329412, 0.329412,
	 0.5f, 0.0f, -0.5f,     0.329412, 0.329412, 0.329412,
	 0.5f, 0.0f,  0.5f,     0.329412, 0.329412, 0.329412,
	 0.0f, 0.8f,  0.0f,     0.329412, 0.329412, 0.329412,
};

GLfloat vertices2[] = {
	// COORDINATES			// COLORS				
	-0.5f, 0.0f,  0.5f,     0.329412, 0.329412, 0.329412,
	-0.5f, 0.0f, -0.5f,     0.329412, 0.329412, 0.329412,
	 0.5f, 0.0f, -0.5f,     0.329412, 0.329412, 0.329412,
	 0.5f, 0.0f,  0.5f,     0.329412, 0.329412, 0.329412,
	 0.0f, 0.8f,  0.0f,     0.329412, 0.329412, 0.329412,
};

GLuint planeIndices[] = {
	0, 2, 1,
	0, 3, 2
};

GLuint indices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 1, 4,
		1, 2, 4,
		2, 3, 4,
		3, 0, 4
};

/*
 *  The loadObject procedure loads an object from an OBJ
 *  file and creates a mesh structure for it.  It also
 *  sets up the buffers for vertex information and indices.
 *  The parameter to this procedure is the file name for the model.
 */
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

/*
 *  The loadProgram procedure reads the vertex and fragment programs
 *  compiles them, and load them to produce the complete GPU program.
 *  The parameters to this procedure and the file names for the vertex
 *  and fragment programs.
*/

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

/*
 *  The copyObject procedure makes a copy of an existing mesh.  This
 *  makes it easier to create multiple copies of an object.  The
 *  paraneter to this procedure is the mesh to be copied.
*/

Mesh* copyObject(Mesh* obj) {
	Mesh* result;

	result = new Mesh();
	*result = *obj;
	return(result);
}

std::vector<Mesh*> buildVillage(Mesh* mesh, GLuint houseProgram, GLuint roofProgram, float rotation, std::vector<float> houseX, std::vector<float> houseZ, float villageStartPosX, float villageStartPosZ, int numHouses, bool logDivider) {
	std::vector<Mesh*> Models;
	mesh = loadOBJ((char*)"./Resource Files/Objects/house.obj");
	// mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(displacement - xPos, 0.1f, zPos));
	// mesh->model = glm::rotate(mesh->model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	mesh->program = houseProgram;
	
	// house row 1
	for (int i = 0; i < numHouses; i += 2) {
		mesh = copyObject(mesh);
		float currX = i - villageStartPosX;
		float currZ = villageStartPosZ;
		houseX.push_back(currX);
		houseZ.push_back(currZ);
		mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(currX, 0.1f, currZ));
		mesh->model = glm::rotate(mesh->model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		Models.push_back(mesh);
	}

	// house 2
	for (int i = 0; i < numHouses; i += 2) {
		mesh = copyObject(mesh);
		float currX = i - villageStartPosX;
		float currZ = villageStartPosZ + 3.0f;
		houseX.push_back(currX);
		houseZ.push_back(currZ);
		mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(currX, 0.1f, currZ));
		mesh->model = glm::rotate(mesh->model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		Models.push_back(mesh);
	}

	// house roof 1
	mesh = loadOBJ((char*)"./Resource Files/Objects/roof.obj");
	mesh->program = roofProgram;

	for (int i = 0; i < houseX.size(); i++) {
		mesh = copyObject(mesh);
		mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(houseX[i], 0.65f, houseZ[i]));
		mesh->model = glm::rotate(mesh->model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		Models.push_back(mesh);
	}

	// fence
	mesh = loadOBJ((char*)"./Resource Files/Objects/fence.obj");
	mesh->program = roofProgram;

	// fence row 1
	for (int i = (villageStartPosX - 7.0f); i < villageStartPosX + 2.0f; i++) {
		mesh = copyObject(mesh);
		mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(i, -0.2f, villageStartPosZ + 4.0f));
		mesh->model = glm::rotate(mesh->model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mesh->model = glm::scale(mesh->model, glm::vec3(1.0f, 0.7f, 1.0f));
		Models.push_back(mesh);
	}

	// fence row 2
	for (int i = (villageStartPosX - 7.0f); i < villageStartPosX + 2.0f; i++) {
		mesh = copyObject(mesh);
		mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(i, -0.2f, villageStartPosZ - 1.0f));
		mesh->model = glm::rotate(mesh->model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mesh->model = glm::scale(mesh->model, glm::vec3(1.0f, 0.7f, 1.0f));
		Models.push_back(mesh);
	}


	// log row in middle of village
	mesh = loadOBJ((char*)"./Resource Files/Objects/log.obj");
	mesh->program = roofProgram;
	if (logDivider) {
		for (int i = 0; i < numHouses; i++) {
			mesh = copyObject(mesh);
			float currX = i - villageStartPosX;
			float currZ = villageStartPosZ;
			mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(currX, -0.1f, (currZ / 2) - 1.0f));
			mesh->model = glm::rotate(mesh->model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			Models.push_back(mesh);
		}
	}
	
	return Models;
}

std::vector<Mesh*> placeTrees(Mesh* mesh, GLuint treeProgram, GLuint leafProgram, char* treeType, char* leafType, std::vector<float> treeX, std::vector<float> treeZ, float mapStartX, float mapStartZ, float mapXRange, float mapZRange, float treeWorldPlacementY, float leafWorldPlacementY) {
	std::vector<Mesh*> Models;
	mesh = loadOBJ((char*)treeType);
	mesh->program = treeProgram;

	for (int x = mapStartX; x < mapXRange; x+=3) {
		for (int z = mapStartZ; z < mapZRange; z+=3) {
			mesh = copyObject(mesh);
			treeX.push_back(x + 0.9f);
			treeZ.push_back(z + 0.9f);
			float RandomDegree = (rand() % 90) + 1;
			mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(glm::vec3(x + 0.9f, treeWorldPlacementY, z + 0.9f)));
			mesh->model = glm::scale(mesh->model, glm::vec3(2.0f, 3.0f, 2.0f));
			mesh->model = glm::rotate(mesh->model, glm::radians(RandomDegree), glm::vec3(0.0f, 1.0f, 0.0));
			Models.push_back(mesh);
		}
	}


	mesh = loadOBJ((char*)leafType);
	mesh->program = leafProgram;

	for (int i = 0; i < treeX.size(); i++) {
		mesh = copyObject(mesh);
		mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(treeX[i], leafWorldPlacementY, treeZ[i]));
		mesh->model = glm::scale(mesh->model, glm::vec3(2.6f, 2.5f, 2.5f));
		Models.push_back(mesh);
	}

	// for (int i = 0; i < numTrees; i++) {
	// 	mesh = copyObject(mesh);
	// 	int RandomX = (rand() % (int)mapStartX) + mapXRange;
	// 	int RandomZ = (rand() % (int)mapStartZ) + mapZRange;
	// 	float RandomDegree = (rand() % 90) + 1;
	// 	treeX.push_back(RandomX);
	// 	treeZ.push_back(RandomZ);
	// 	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(RandomX, treeWorldPlacementY, RandomZ));
	// 	mesh->model = glm::scale(mesh->model, glm::vec3(2.0f, 3.0f, 2.0f));
	// 	mesh->model = glm::rotate(mesh->model, glm::radians(RandomDegree), glm::vec3(0.0f, 1.0f, 0.0));
	// 	Models.push_back(mesh);
	// }
	// 
	// mesh = loadOBJ((char*)leafType);
	// mesh->program = leafProgram;
	// 
	// for (int i = 0; i < treeX.size(); i++) {
	// 	mesh = copyObject(mesh);
	// 	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(treeX[i], leafWorldPlacementY, treeZ[i]));
	// 	if (leafType == "./Resource Files/Objects/pine.obj") mesh->model = glm::scale(mesh->model, glm::vec3(2.6f, 2.5f, 2.5f));
	// 	if (leafType == "./Resource Files/Objects/leaf.obj") mesh->model = glm::scale(mesh->model, glm::vec3(2.5f, 2.5f, 2.5f));
	// 	Model.push_back(mesh);
	// }

	return Models;
}


/*
 *  The buildModel procedure constructs the scene from the indivdual meshes.
*/
void buildModel() {
	Mesh* mesh;
	GLuint program;
	GLuint program2;
	GLuint program3;
	GLuint program4;
	GLuint program5;
	GLuint program6;
	// store tree locations [x, y]
	std::vector<int> treeX;
	std::vector<int> treeZ;

	std::vector<float> treetrunkX;
	std::vector<float> treetrunkZ;

	std::vector<float> villageHouseX;
	std::vector<float> villageHouseZ;

	std::vector<Mesh*> village1;
	std::vector<Mesh*> village2;
	std::vector<Mesh*> village3;

	std::vector<Mesh*> trees;

	// Use the same GPU program for all of the meshes.
	program = loadProgram((char*)"./Resource Files/Shaders/model.vs", (char*)"./Resource Files/Shaders/model.fs");
	program2 = loadProgram((char*)"./Resource Files/Shaders/ground.vs", (char*)"./Resource Files/Shaders/ground.fs");
	program3 = loadProgram((char*)"./Resource Files/Shaders/castle.vs", (char*)"./Resource Files/Shaders/castle.fs");
	program4 = loadProgram((char*)"./Resource Files/Shaders/rock.vs", (char*)"./Resource Files/Shaders/rock.fs");
	program5 = loadProgram((char*)"./Resource Files/Shaders/leaf.vs", (char*)"./Resource Files/Shaders/leaf.fs");
	program6 = loadProgram((char*)"./Resource Files/Shaders/house.vs", (char*)"./Resource Files/Shaders/house.fs");

	planeShaderProgram = loadProgram((char*)"./Resource Files/Shaders/plane.vs", (char*)"./Resource Files/Shaders/plane.fs");
	pyramidShaderProgram = loadProgram((char*)"./Resource Files/Shaders/pyramid.vs", (char*)"./Resource Files/Shaders/pyramid.fs");


	// Start with the bunny, rotate it so it has the same orientation as the monkey

	// load world tree
	mesh = loadOBJ((char*)"./Resource Files/Objects/worldtree.obj");
	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.25f, -4.5f));
	mesh->model = glm::scale(mesh->model, glm::vec3(4.2f, 4.2f, 4.2f));
	mesh->program = program;
	Model.push_back(mesh);

	// pine tree Left of village in front of castle
	trees = placeTrees(mesh, program, program5, "./Resource Files/Objects/trunk.obj", "./Resource Files/Objects/pine.obj", treetrunkX, treetrunkZ, 4, 7, 24, 24, -0.45f, 0.0f);
	for (int i = 0; i < trees.size(); i++) {
		Model.push_back(trees[i]);
	}

	trees.clear();
	treetrunkX.clear();
	treetrunkZ.clear();

	trees = placeTrees(mesh, program, program5, "./Resource Files/Objects/trunk.obj", "./Resource Files/Objects/pine.obj", treetrunkX, treetrunkZ, -24, 7, -4, 24, -0.45f, 0.0f);
	for (int i = 0; i < trees.size(); i++) {
		Model.push_back(trees[i]);
	}


	trees.clear();
	treetrunkX.clear();
	treetrunkZ.clear();

	// oak trees
	trees = placeTrees(mesh, program, program5, "./Resource Files/Objects/lowpolytree.obj", "./Resource Files/Objects/leaf.obj", treetrunkX, treetrunkZ, -24, 1, 24, 7, -0.3f, 1.8f);
	for (int i = 0; i < trees.size(); i++) {
		Model.push_back(trees[i]);
	}

	trees.clear();
	treetrunkX.clear();
	treetrunkZ.clear();

	
	// village 1 (left of castle entrance)
	village1 = buildVillage(mesh, program6, program, 180.0f, villageHouseX, villageHouseZ, 13.0f, -5.0f, 8, true);
	for (int i = 0; i < village1.size(); i++) {
		Model.push_back(village1[i]);
	}

	// reset house roof position vector
	villageHouseX.clear();
	villageHouseZ.clear();

	// village 2 (right of case entrance)
	village2 = buildVillage(mesh, program6, program, 180.0f, villageHouseX, villageHouseZ, -7.0f, -5.0f, 8, true);
	for (int i = 0; i < village2.size(); i++) {
		Model.push_back(village2[i]);
	}

	villageHouseX.clear();
	villageHouseZ.clear();
	
	village3 = buildVillage(mesh, program6, program, 180.0f, villageHouseX, villageHouseZ, 2.5f, 10.0f, 4, false);
	for (int i = 0; i < village3.size(); i++) {
		Model.push_back(village3[i]);
	}

	// load mountain
	mesh = loadOBJ((char*)"./Resource Files/Objects/mountain2.obj");
	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, -30.0f));
	mesh->model = glm::scale(mesh->model, glm::vec3(100.0f, 35.0f, 35.00f));
	mesh->model = glm::rotate(mesh->model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mesh->program = program4;
	Model.push_back(mesh);

	// mountain right of castle entrance
	mesh = copyObject(mesh);
	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(42.0f, -0.1999f, 0.0f));
	mesh->model = glm::scale(mesh->model, glm::vec3(35.0f, 35.0f, 60.00f));
	mesh->model = glm::rotate(mesh->model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Model.push_back(mesh);

	// mountain left of castle entrance
	//mesh = loadOBJ((char*)"mountain.obj");
	mesh = copyObject(mesh);
	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(-42.0f, -0.2f, 0.0f));
	mesh->model = glm::scale(mesh->model, glm::vec3(35.0f, 35.0f, 60.00f));
	mesh->model = glm::rotate(mesh->model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//mesh->program = program4;
	Model.push_back(mesh);

	// mountain back
	mesh = copyObject(mesh);
	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, 42.0f));
	mesh->model = glm::scale(mesh->model, glm::vec3(120.0f, 35.0f, 35.00f));
	mesh->model = glm::rotate(mesh->model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Model.push_back(mesh);

	// load log
	mesh = loadOBJ((char*)"./Resource Files/Objects/log.obj");
	//mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -0.1f, 7.0f));
	mesh->program = program;
	//Model.push_back(mesh);

	// forest village
	for (int i = 0; i < 3; i++) {
		mesh = copyObject(mesh);
		mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(i + 1.5f, -0.1f, 10.0f));
		Model.push_back(mesh);
	}
	for (int i = 0; i < 3; i++) {
		mesh = copyObject(mesh);
		mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(i + 2.0f, -0.1f, 10.0f));
		Model.push_back(mesh);
	}

	// load castle
	mesh = loadOBJ((char*)"./Resource Files/Objects/castle.obj");
	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, -4.0f));
	mesh->model = glm::scale(mesh->model, glm::vec3(10.1f, 10.1f, 10.1f));
	mesh->program = program3;
	Model.push_back(mesh);

	// load castle fountain
	mesh = loadOBJ((char*)"./Resource Files/Objects/castlefountain.obj");
	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, -4.5f));
	mesh->model = glm::scale(mesh->model, glm::vec3(3.0f, 1.0f, 3.0f));
	mesh->program = program4;
	Model.push_back(mesh);

	// load grass
	mesh = loadOBJ((char*)"./Resource Files/Objects/singlegrass.obj");
	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, -5.0f));
	mesh->model = glm::scale(mesh->model, glm::vec3(0.3f, 0.3f, 0.3f));
	mesh->program = program2;
	Model.push_back(mesh);

	for (int i = -24; i < 24; i++) {
		for (int j = -13; j < 24; j++) {
			mesh = copyObject(mesh);
			mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(i + 0.9f, -0.2f, j + 0.9f));
			mesh->model = glm::scale(mesh->model, glm::vec3(0.3f, 0.3f, 0.3f));
			Model.push_back(mesh);
		}
	}

	// load plane
	// mesh = loadOBJ((char*)"./Resource Files/Objects/plane.obj");
	/*mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, -0.0f));
	mesh->model = glm::scale(mesh->model, glm::vec3(60.1f, 1.0f, 50.1f));
	mesh->model = glm::rotate(mesh->model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	mesh->program = program2;
	Model.push_back(mesh);*/
}

// preserves aspect ratio when resizing window
void framebufferSizeCallback(GLFWwindow* window, int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0) h = 1;

	float ratio = 1.0f * w / h;

	glfwMakeContextCurrent(window);

	glViewport(0, 0, w, h);

	projection = glm::perspective(60.0f, ratio, 1.0f, 800.0f);
}

/*
 *  This procedure is called each time the screen needs
 *  to be redisplayed
 */
void display() {
	glm::mat4 view;
	glm::mat4 modelViewPerspective;
	glm::mat3 normal;
	int modelLoc;
	int normalLoc;
	int i;
	Mesh* mesh;
	GLuint vPosition;
	GLuint vNormal;

	//view = camera.GetViewMatrix();
	view = camera.getViewMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw Pyramid 1
	VAO VAO1;
	VAO1.Bind();
	
	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));
	
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 6 * sizeof(float), (void*)(6 * sizeof(float)));
	
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();
	
	glUseProgram(pyramidShaderProgram);
	
	glm::mat4 modelp = glm::mat4(1.0f);
	glm::mat4 viewp = camera.getViewMatrix();
	glm::mat4 projp = glm::mat4(1.0f);
	
	modelp = glm::rotate(modelp, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	modelp = glm::scale(modelp, glm::vec3(3.0f, 3.0f, 3.0f));
	viewp = glm::translate(viewp, glm::vec3(0.0f, 2.4f + pyramidFloat, -7.5f));
	projp = camera.getCameraPerspective();
	
	int modelLocp = glGetUniformLocation(planeShaderProgram, "model");
	glUniformMatrix4fv(modelLocp, 1, GL_FALSE, glm::value_ptr(modelp));
	int viewLocp = glGetUniformLocation(planeShaderProgram, "view");
	glUniformMatrix4fv(viewLocp, 1, GL_FALSE, glm::value_ptr(viewp));
	int projLocp = glGetUniformLocation(planeShaderProgram, "proj"); 
	glUniformMatrix4fv(projLocp, 1, GL_FALSE, glm::value_ptr(projp));
	
	VAO1.Bind();
	
	glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(int), GL_UNSIGNED_INT, 0);

	// Draw pyramid 2
	VAO VAO3;
	VAO3.Bind();

	VBO VBO3(vertices, sizeof(vertices2));
	EBO EBO3(indices, sizeof(indices));

	VAO3.LinkAttrib(VBO3, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO3.LinkAttrib(VBO3, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO3.LinkAttrib(VBO3, 2, 2, GL_FLOAT, 6 * sizeof(float), (void*)(6 * sizeof(float)));

	VAO3.Unbind();
	VBO3.Unbind();
	EBO3.Unbind();


	modelp = glm::mat4(1.0f);
	viewp = camera.getViewMatrix();
	projp = glm::mat4(1.0f);

	modelp = glm::rotate(modelp, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelp = glm::rotate(modelp, glm::radians(-rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	modelp = glm::scale(modelp, glm::vec3(3.0f, 3.0f, 3.0f));
	viewp = glm::translate(viewp, glm::vec3(0.0f, 2.4f + pyramidFloat, -7.5f));
	projp = camera.getCameraPerspective();

	modelLocp = glGetUniformLocation(planeShaderProgram, "model");
	glUniformMatrix4fv(modelLocp, 1, GL_FALSE, glm::value_ptr(modelp));
	viewLocp = glGetUniformLocation(planeShaderProgram, "view");
	glUniformMatrix4fv(viewLocp, 1, GL_FALSE, glm::value_ptr(viewp));
	projLocp = glGetUniformLocation(planeShaderProgram, "proj");
	glUniformMatrix4fv(projLocp, 1, GL_FALSE, glm::value_ptr(projp));

	VAO3.Bind();

	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
	
	int pyramidLocation = glGetUniformLocation(pyramidShaderProgram, "u_pyramidColor");
	glUniform4f(pyramidLocation, 0.0f, 0.5f + pyramidFloat, 1.0f, 1.0f);

	// plane
	VAO VAO2;
	VAO2.Bind();
	
	// plane
	VBO VBO2(planeVertices, sizeof(planeVertices));
	EBO EBO2(planeIndices, sizeof(planeIndices));
	
	// plane
	VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO2.LinkAttrib(VBO2, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	
	// plane
	VAO2.Unbind();
	VBO2.Unbind();
	EBO2.Unbind();
	
	glUseProgram(planeShaderProgram);
	
	glm::mat4 modelpl = glm::mat4(1.0f);
	glm::mat4 viewpl = camera.getViewMatrix();
	glm::mat4 projpl = glm::mat4(1.0f);
	
	modelpl = glm::rotate(modelpl, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelpl = glm::scale(modelpl, glm::vec3(60.0f, 60.0f, 1.0f));
	viewpl = glm::translate(viewpl, glm::vec3(0.0f, -0.199f, -1.5f));
	projpl = camera.getCameraPerspective();
	
	int modelLocpl = glGetUniformLocation(planeShaderProgram, "model");
	glUniformMatrix4fv(modelLocpl, 1, GL_FALSE, glm::value_ptr(modelpl));
	int viewLocpl = glGetUniformLocation(planeShaderProgram, "view");
	glUniformMatrix4fv(viewLocpl, 1, GL_FALSE, glm::value_ptr(viewpl));
	int projLocpl = glGetUniformLocation(planeShaderProgram, "proj");
	glUniformMatrix4fv(projLocpl, 1, GL_FALSE, glm::value_ptr(projpl));
	
	VAO2.Bind();
	
	glDrawElements(GL_TRIANGLES, sizeof(planeIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
	
	int planeLocation = glGetUniformLocation(planeShaderProgram, "u_planeColor");
	glUniform4f(planeLocation, 86.0f/255.0f, 125.0f/255.0f, 70.0f/255.0f, 1.0f);

	// Iterate through the model displaying each of the meshes
	for (i = 0; i < Model.size(); i++) {
		mesh = Model[i];
		glUseProgram(mesh->program);
		
		// Set up the position and normal attributes
		glBindBuffer(GL_ARRAY_BUFFER, mesh->vbuffer);
		vPosition = glGetAttribLocation(mesh->program, "vPosition");
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(vPosition);
		vNormal = glGetAttribLocation(mesh->program, "vNormal");
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(mesh->vbytes));
		glEnableVertexAttribArray(vNormal);

		// Set up the transformation matrices for the vertices and normal vectors

		modelLoc = glGetUniformLocation(mesh->program, "model");
		modelViewPerspective = projection * view * mesh->model;
		glUniformMatrix4fv(modelLoc, 1, 0, glm::value_ptr(modelViewPerspective));
		normalLoc = glGetUniformLocation(mesh->program, "normalMat");
		normal = glm::transpose(glm::inverse(glm::mat3(view * mesh->model)));
		glUniformMatrix3fv(normalLoc, 1, 0, glm::value_ptr(normal));

		// Finally draw the mesh
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibuffer);
		glDrawElements(GL_TRIANGLES, 3 * mesh->triangles, GL_UNSIGNED_INT, NULL);
		
		int loc = glGetUniformLocation(mesh->program, "u_Color");
		glUniform4f(loc, 0.35f, 0.16f, 0.14f, 1.0f);

		int groundColLocation = glGetUniformLocation(mesh->program, "u_groundColor");
		glUniform4f(groundColLocation, 86.0f/255.0f, 125.0f/255.0f, 70.0f/255.0f, 1.0f);

		int castleLoc = glGetUniformLocation(mesh->program, "u_castleColor");
		glUniform4f(castleLoc, 0.184314f, 0.309804f, 0.309804f, 1.0f);

		int rockLoc = glGetUniformLocation(mesh->program, "u_rockColor");
		glUniform4f(rockLoc, 0.329412, 0.329412, 0.329412, 1.0f);

		int leafLoc = glGetUniformLocation(mesh->program, "u_leafColor");
		glUniform4f(leafLoc, 86.0f / 255.0f, 125.0f / 255.0f, 70.0f / 255.0f, 1.0f);

		int houseLoc = glGetUniformLocation(mesh->program, "u_houseColor");
		glUniform4f(houseLoc, 0.858824f, 0.576471f, 0.439216f, 1.0f);
	}
}

// process keyboard inputs
void processInputs(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char** argv) {
	//srand(static_cast <unsigned> (time(0)));
	float rate = 0.005f;
	float dfloat = 0.005f;

	float skyfloat = 0.0005f;
	// initialize glfw
	glfwInit();
	// create window
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Assignment 1", NULL, NULL);
	float sky = 0.0f;
	// tell glfw what version of opengl we are using (version 3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// tell glfw we are using the CORE profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// MSAA
	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwSetErrorCallback(error_callback);

	// initialize glfw
	if (!glfwInit()) {
		fprintf(stderr, "can't initialize GLFW\n");
	}

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// initialize glew	
	glfwMakeContextCurrent(window);
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		printf("Error starting GLEW: %s\n", glewGetErrorString(error));
		exit(0);
	}

	// MSAA anti aliasing
	glEnable(GL_MULTISAMPLE);

	/*glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);*/

	// build all models
	buildModel();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// projection matrix init
	projection = glm::perspective(60.0f, 1.0f, 1.0f, 500.0f);

	double previousTime = 0.0;
	double currentTime = 0.0;
	double differenceTime;
	unsigned counter = 0;

	glfwSwapInterval(1);

	// GLFW main loop, display model, swapbuffer and check for input
	while (!glfwWindowShouldClose(window)) {
		rotation += 0.5f;

		pyramidFloat += dfloat;
		if (pyramidFloat > 1.0f) {
			
			dfloat = -rate;
		}
		if (pyramidFloat < -0.5f) {
			dfloat = rate;
		}
		// simple timer
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		currentTime = glfwGetTime();
		differenceTime = currentTime - previousTime;
		counter++;
		if (differenceTime >= 1.0 / 30.0) {
			std::string FPS = std::to_string((1.0 / differenceTime) * counter);
			std::string newTitle = "Assignment 1 - " + FPS + " FPS";
			glfwSetWindowTitle(window, newTitle.c_str());
			previousTime = currentTime;
			counter = 0;
		}

		// set projection to camera perspective matrix
		projection = camera.getCameraPerspective();
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f + sky, 1.0f);
		// extra - added background color change to give night -> day sky illusion
		sky += skyfloat;
		if (sky > 1.0f) {
			skyfloat = -0.0005f;
		}
		if (sky < 0.0f) {
			skyfloat = 0.0005f;
		}
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// display 
		display();
		// process keyboard inputs
		processInputs(window);
		camera.keyboardInputs(window);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}
