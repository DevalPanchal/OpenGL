#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


// Inputs the color from the Vertex Shader
in vec3 color;

uniform vec4 u_pyramidColor;

void main()
{
	vec3 N;
	vec3 L = vec3(0.0, 0.0, 1.0);
	vec4 colour = u_pyramidColor;
	float diffuse;
	
	N = normalize(color);
	L = normalize(L);
	diffuse = dot(N,L);
	if(diffuse < 0.0) {
		diffuse = 0.0;
	}
	
	gl_FragColor = min(0.3*colour + 0.7*diffuse*colour, vec4(1.0));
	gl_FragColor.a = colour.a;

	// FragColor = vec4(bcolour, 1.0f);
	// FragColor = vec4(color, 1.0f);
}