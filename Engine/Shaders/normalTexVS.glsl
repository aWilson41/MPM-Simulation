#version 460
uniform mat4 mvp_matrix;

in vec3 inPos;
in vec3 inNormal;
in vec2 inTexCoord;

smooth out vec3 normal;
smooth out vec2 texCoord;

void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * vec4(inPos, 1.0);
	normal = inNormal;
	texCoord = inTexCoord;
}