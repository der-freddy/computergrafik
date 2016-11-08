#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;
uniform vec3 PlanetColor;
uniform float Glossyness;
out vec3 planetPosition;

out vec3 pass_Normal;
out vec3 pass_Color;
out float pass_Glossyness;
out vec3 pass_lightRay;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;

	planetPosition = vec3(ModelMatrix * vec4(in_Position, 1.0));
	pass_lightRay = vec3(0.0, 0.0, 0.0) - planetPosition;
	pass_Color = PlanetColor;
	pass_Glossyness = Glossyness;
}
