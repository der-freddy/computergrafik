#version 150
#extension GL_ARB_explicit_attrib_location : require

uniform sampler2D ColorTex;


uniform uint Lum;
uniform uint Hor;
uniform uint Vert;
uniform uint Gau;

in vec2 pass_TexCoord;

out vec4 out_Color;



void main(void)
{

	float x = pass_TexCoord.x;
	float y = pass_TexCoord.y;
	out_Color = texture(ColorTex, pass_TexCoord);
	vec4 color    = vec4(1.0f);

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

	

		if(Gau == uint(1)){
	
		vec2 offset = pass_TexCoord / gl_FragCoord.xy;
	    vec2 pixel[9];
		pixel[0] = vec2(x - offset.x, y + offset.y );
		pixel[1] = vec2(x           , y + offset.y );
		pixel[2] = vec2(x + offset.x, y + offset.y );
		pixel[3] = vec2(x - offset.x, y            );
		pixel[4] = vec2(x           , y            );
		pixel[5] = vec2(x + offset.x, y            );
		pixel[6] = vec2(x - offset.x, y - offset.y );
		pixel[7] = vec2(x           , y - offset.y );
		pixel[8] = vec2(x + offset.x, y - offset.y );
	
	
	    vec4 cul = texture(ColorTex, pixel[0]) * 1/16;
	    vec4 cum = texture(ColorTex, pixel[1]) * 1/8;
	    vec4 cur = texture(ColorTex, pixel[2]) * 1/16;
	    vec4 cml = texture(ColorTex, pixel[3]) * 1/8;
	    vec4 cmm = texture(ColorTex, pixel[4]) * 1/4;
	    vec4 cmr = texture(ColorTex, pixel[5]) * 1/8;
	    vec4 cbl = texture(ColorTex, pixel[6]) * 1/16;
	    vec4 cbm = texture(ColorTex, pixel[7]) * 1/8;
	    vec4 cbr = texture(ColorTex, pixel[8]) * 1/16;
	
	
	    out_Color = cul + cum + cur + cml + cmm + cmr + cbl + cbm + cbr;



	}


if(Lum == uint(1)){

		float Lum = 0.2126 *out_Color.r + 0.7152*out_Color.r  + 0.0722*out_Color.r;

		out_Color = vec4(Lum,Lum, Lum, 1.0f);
	}

}