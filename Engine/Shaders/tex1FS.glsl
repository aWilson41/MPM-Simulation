#version 460
uniform sampler2D tex;

smooth in vec2 texCoord;

out vec4 fColor;

void main()
{
	vec4 val = texture2D(tex, texCoord);
	fColor = vec4(val.x, val.x, val.x, 1.0f);
}