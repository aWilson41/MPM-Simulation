#version 460
struct Material
{
	vec3 diffuseColor;
	vec3 ambientColor;
};
uniform Material mat;

out vec4 fColor;

void main()
{
	fColor = vec4(mat.ambientColor, 1.0);
}