#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 bColor;

// Outputs the color for the Fragment Shader
out vec3 color;
out vec3 bcolor;

// Controls the scale of the vertices
uniform float scale;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	// Assigns the colors from the Vertex Data to "color"
	color = aColor;
	bcolor = bColor;
}