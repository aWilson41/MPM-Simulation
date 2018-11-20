#version 460
uniform mat4 mvp_matrix;

in vec3 inPos;

out float gl_PointSize;

void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * vec4(inPos, 1.0);

	gl_PointSize = 4;
}