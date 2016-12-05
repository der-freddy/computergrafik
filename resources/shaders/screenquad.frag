#version 150
#extension GL_ARB_explicit_attrib_location : require

uniform sampler2D ColorTex;


uniform uint Lum;
uniform uint Hor;
uniform uint Vert;

in vec2 pass_TexCoord;

out vec4 out_Color;

void main(void)
{

	float x = pass_TexCoord.x;
	float y = pass_TexCoord.y;
	out_Color = texture(ColorTex, pass_TexCoord);

	if(Hor == uint(1)){
		x = x;
		y = 1-y;
		out_Color = texture(ColorTex, vec2(x,y));
	}
	if(Vert == uint(1)){
		x = (1 - x);
		y = y;
		out_Color = texture(ColorTex, vec2(x,y));
	}

	if(Lum == uint(1)){

		float Lum = 0.2126 *out_Color.r + 0.7152*out_Color.r  + 0.0722*out_Color.r;

		out_Color = vec4(Lum,Lum, Lum, 1.0f);
	}
}