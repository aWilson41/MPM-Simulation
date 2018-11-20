#version 460
uniform mat4 mvp_matrix;

in vec3 inPos;
in vec2 texCoord;

out float gl_PointSize;
out vec2 vTexCoord;

void main()
{
	// Calculate vertex position in screen space
	gl_Position = mvp_matrix * vec4(inPos, 1.0);
}