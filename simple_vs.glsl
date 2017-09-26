#version 400

in vec3 vp;
in vec3 vcol;
out vec4 color;
uniform mat4 uModelViewMatrix;

void main () 
{
  gl_Position = uModelViewMatrix * vec4 (vp, 1.0);
  color = vec4(vcol, 1.0);
}
