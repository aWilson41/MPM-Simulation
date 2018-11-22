#version 460
struct Material
{
	vec3 diffuseColor;
	vec3 ambientColor;
};

uniform Material mat;
uniform vec3 lightDir;

smooth in vec3 normal;

out vec4 fColor;

void main()
{
	vec3 color = clamp(dot(lightDir, normal) * mat.diffuseColor + mat.ambientColor, 0.0f, 1.0f);
	fColor = vec4(color, 1.0f);
	//fColor = vec4(normal, 1.0);
}