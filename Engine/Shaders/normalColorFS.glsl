#version 460
uniform vec3 lightDir;

smooth in vec3 normal;
smooth in vec3 color;

out vec4 fColor;

void main()
{
	vec3 color = clamp(dot(lightDir, normal) * color, 0.0f, 1.0f);
	fColor = vec4(color, 1.0f);
}