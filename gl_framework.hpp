#ifndef _GL_FRAMEWORK_HPP_
#define _GL_FRAMEWORK_HPP_

#define CI_MODELLING_MODE 0
#define CI_INSPECTION_MODE 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <GL/glew.h>

#include <iostream>
#include <vector>
#include <string>

#include <fstream>

namespace csX75
{
  //! Initialize GL State
  void initGL(void);
 
  //!GLFW Error Callback
  void error_callback(int error, const char* description);
  //!GLFW framebuffer resize callback
  void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
  //!GLFW mouse click callback
  void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};

#endif
