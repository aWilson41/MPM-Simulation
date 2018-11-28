#version 460

smooth in vec3 color;
out vec4 fColor;

void main()
{
	fColor = vec4(color, 1.0);
}