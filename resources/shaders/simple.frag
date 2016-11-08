#version 150

in  vec3 pass_Normal;
in  vec3 pass_Color;
in  float pass_Glossyness;
in  vec3 pass_lightRay;


out  vec4 out_Color;

void main() {

	vec3 pass_ambient = pass_Color * 0.4;
	vec3 pass_diffuse = pass_Color * 0.6;
	vec3 is = vec3(0.5f, 0.5f, 0.5f);

	vec3 pass_normal_n = normalize(pass_Normal);
	vec3 pass_lighRay_n = (pass_lightRay);
  	out_Color = vec4(pass_ambient, 1.0) + vec4(pass_diffuse * max(0.0, dot(pass_normal_n, pass_lighRay_n)), 1.0) * pass_Glossyness;

  	vec3   S = pass_color * is;
    float sf = max(0.0f, dot(normal.xyz, h));
          sf = pow(sf, pass_Glossyness);

}
