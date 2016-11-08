#version 150

in  vec3 pass_Normal;
in  vec3 pass_Color;
in  float pass_Glossyness;
in  vec3 pass_lightRay;
in  vec3 pass_Viewer;


out  vec4 out_Color;

void main() {
	vec3 ka = pass_Color;
	vec3 kd = pass_Color;
	vec3 ks = vec3(1.0, 1.0, 1.0);

	vec3 ia = vec3(0.4, 0.4, 0.4);
	vec3 id = vec3(0.4, 0.4, 0.4);
	vec3 is = vec3(0.6, 0.6, 0.6);

	vec3 h = normalize(pass_lightRay + pass_Viewer);

	vec3 a = ka * ia;
	vec3 d = kd * id * max(0.0f, dot(normalize(pass_Normal), pass_lightRay));
	float sTemp = max(0.0f, dot(normalize(pass_Normal), h));
	vec3 s = ks * is * pow(sTemp, pass_Glossyness);

  
  	out_Color = vec4(a + d + s, 1.0f);

}
