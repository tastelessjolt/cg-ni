#version 400

in vec3 vp;
in vec3 vcol;
out vec4 color;
uniform mat4 uModelViewMatrix;
uniform int mode;

void main () 
{
  vec4 homo_coord = uModelViewMatrix * vec4 (vp, 1.0) ;
  gl_Position = mode != 2 ? homo_coord : vec4(vec3(homo_coord), 1.0);
  color = vec4(vcol, 1.0);
}
