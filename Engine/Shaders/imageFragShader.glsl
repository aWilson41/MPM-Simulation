#version 460

uniform sampler2D tex;

in varying vec2 vTexCoord;

out vec4 fColor;

void main()
{
	fColor = texture2D(tex, vTexCoord).rgba;
}