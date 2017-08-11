#version 400

in vec3 vp;
uniform mat4 uModelViewMatrix;

void main () 
{
  gl_Position = uModelViewMatrix * vec4 (vp, 1.0) ;
}
