#version 460
uniform mat4 mvp_matrix;

in vec3 inPos;
in vec3 inScalars;

smooth out vec3 color;

void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * vec4(inPos, 1.0);
	color = inScalars;
}