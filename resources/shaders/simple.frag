#version 150

in  vec3 pass_Normal;
in  vec3 pass_Color;
in  float pass_Glossyness;
in  vec3 pass_lightRay;
in  vec3 pass_Viewer;
flat in uint  pass_Shader; 


out  vec4 out_Color;

void main() {
	vec3 ka = pass_Color;
	vec3 kd = pass_Color;
	vec3 ks = vec3(1.0, 1.0, 1.0);

	if(pass_Shader == uint(0)){

		vec3 ia = vec3(0.4, 0.4, 0.4);
		vec3 id = vec3(0.4, 0.4, 0.4);
		vec3 is = vec3(0.6, 0.6, 0.6);
	
		vec3 h = normalize(pass_lightRay + pass_Viewer);
		//Ambient
		vec3 a = ka * ia;
		//Diffuse
		vec3 d = kd * id * max(0.0f, dot(normalize(pass_Normal), pass_lightRay));
		float sTemp = max(0.0f, dot(normalize(pass_Normal), h));
		//Specular
		vec3 s = ks * is * pow(sTemp, pass_Glossyness*2.5);

	  
	  	out_Color = vec4(a + d + s, 1.0f);
  	}


	else
	{
	    vec3 ia = vec3(0.1f, 0.1f, 0.1f);
		vec3 id = vec3(0.5f, 0.5f, 0.5f);
		vec3 is = vec3(0.5f, 0.5f, 0.5f);

		float shininess = 4.0f;
		float factor  = 5.0f;

		vec3 h = normalize(pass_lightRay.xyz + pass_Viewer);

		// angle
		float outlineAngle = dot(pass_Normal, pass_Viewer);

		//border
		if ( (outlineAngle >= 0.0f) && (outlineAngle <= 0.5f)) {
			out_Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		} 
		else
		{

			// Ambient
			vec3   A = ka * ia;

			// Defuse 
			vec3   D = kd * id;
			float df = max(0.0f, dot(pass_lightRay, pass_Normal) );

			// Specular
			vec3   S = ks * is;
			float sf = max(0.0f, dot(pass_Normal, h));
			sf = pow(sf,  pass_Glossyness);

			df = ceil(df * factor) / factor;
			sf = ceil(sf * factor) / factor;

			out_Color = vec4(A + df * D + sf * S, 1.0f);
  		}
	}
}