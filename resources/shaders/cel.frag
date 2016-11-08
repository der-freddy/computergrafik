#version 150
in  vec3 pass_Normal;
in  vec3 pass_Color;
in  vec3 pass_LightVector;
in  vec3 pass_ViewerVector;

out vec4 out_Color;

void main(void)
{
    vec3 ka = pass_Color;
    vec3 kd = pass_Color;
    vec3 ks = vec3(1.0f, 1.0f, 1.0f);

    vec3 ia = vec3(0.1f, 0.1f, 0.1f);
    vec3 id = vec3(0.5f, 0.5f, 0.5f);
    vec3 is = vec3(0.5f, 0.5f, 0.5f);

    float shininess = 4.0f;
    float factor  = 5.0f;

    vec3 h = normalize(pass_LightVector.xyz + pass_ViewerVector);

    // angle
    float outlineAngle = dot(pass_Normal, pass_ViewerVector);

    //border
    if ( (outlineAngle >= 0.0f) && (outlineAngle <= 0.3f)) {
      out_Color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    } 
    else{
      
      // Ambient Light
      vec3   A = ka * ia;
      
      // Defuse Light
      vec3   D = kd * id;
      float df = max(0.0f, dot(pass_LightVector, pass_Normal) );
      
      // Specular Light
      vec3   S = ks * is;
      float sf = max(0.0f, dot(pass_Normal, h));
            sf = pow(sf, shininess);

      df = ceil(df * factor) / factor;
      sf = ceil(sf * factor) / factor;

      out_Color = vec4(A + df * D + sf * S, 1.0f);
    }
}