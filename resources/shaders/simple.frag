#version 150

in  vec3 pass_Normal;
out vec4 out_Color;

void main() {
  out_Color = pass_Normal;
}
