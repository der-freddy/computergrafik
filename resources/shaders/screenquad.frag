#version 150
#extension GL_ARB_explicit_attrib_location : require

uniform sampler2D ColorTex;

in vec2 pass_TexCoord;
out vec4 out_Color;

void main(void)
{
	out_Color = texture(ColorTex, pass_TexCoord);
}