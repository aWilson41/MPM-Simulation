#version 460
uniform sampler2D tex;
uniform vec3 lightDir;

smooth in vec3 normal;
smooth in vec2 texCoord;

out vec4 fColor;

void main()
{
	vec3 color = clamp(dot(lightDir, normal) * vec3(texture2D(tex, texCoord)), 0.0f, 1.0f);
	fColor = vec4(color, 1.0f);
}