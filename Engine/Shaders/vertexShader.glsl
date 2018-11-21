#version 460
uniform mat4 mvp_matrix;

in vec3 inPos;
in vec2 inColor;
in vec3 inNormal;

smooth out vec3 normal;

void main()
{
	normal = inNormal;

    // Calculate vertex position in screen space
	gl_Position = mvp_matrix * vec4(inPos, 1.0);
}