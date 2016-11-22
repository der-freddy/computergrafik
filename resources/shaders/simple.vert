#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;
uniform vec3 PlanetColor;
uniform float Glossyness;
uniform vec3 SunPosition;
uniform uint Shader;
uniform sampler2d ColorTex;

out vec3 planetPosition;
out vec3 pass_Normal;
out vec3 pass_Color;
out float pass_Glossyness;
out vec3 pass_lightRay;
out vec3 pass_Viewer;
out vec2 pass_TexCoord;
flat out uint pass_Shader;

void main(void)
{
	gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = normalize(NormalMatrix * vec4(in_Normal, 0.0)).xyz;


	planetPosition = vec3((ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0));
	pass_lightRay = normalize(SunPosition - planetPosition.xyz);

	pass_Viewer = normalize(-planetPosition.xyz);

	pass_Color = PlanetColor;
	pass_Glossyness = Glossyness;

	pass_Shader = Shader;
	pass_TexCoord = in_TexCoord;
}