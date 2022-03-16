#version 330 core
/*
 *  Simple fragment sharder for laboratory two
 */

 layout(location = 0) out vec4 color;
in vec3 normal;

uniform vec4 u_Color;
uniform vec4 u_pineColor;

void main() {
	vec3 N;
	vec3 L = vec3(1.0, 1.0, 0.0);
	vec4 colour = u_Color;
	float diffuse;

	N = normalize(normal);
	L = normalize(L);
	diffuse = dot(N,L);
	if(diffuse < 0.0) {
		diffuse = 0.0;
	}

	gl_FragColor = min(0.3*colour + 0.7*diffuse*colour, vec4(1.0));
	gl_FragColor.a = colour.a;
}