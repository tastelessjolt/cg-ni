#include "gl_framework.hpp"

extern GLfloat zrot;
extern float points[];

namespace csX75
{
  //! Initialize GL State
  void initGL(void)
  {
    //Set framebuffer clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //Set depth buffer furthest depth
    glClearDepth(1.0);
    //Set depth test to less-than
    glDepthFunc(GL_LESS);
    //Enable depth testing
    glEnable(GL_DEPTH_TEST); 
    //Enable Gourard shading
    glShadeModel(GL_SMOOTH);
  }
  
  //!GLFW Error Callback
  void error_callback(int error, const char* description)
  {
    std::cerr<<description<<std::endl;
  }
  
  //!GLFW framebuffer resize callback
  void framebuffer_size_callback(GLFWwindow* window, int width, int height)
  {
    //!Resize the viewport to fit the window size - draw to entire window
    glViewport(0, 0, width, height);
  }
  
  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    //!Close the window if the ESC key was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      points[0] += zrot;
      points[1] += zrot;
      points[2] += zrot;
      points[3] += zrot;
      points[4] += zrot;
      points[5] += zrot;
      zrot += 0.3;
    } 
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      points[0] -= zrot;
      points[1] -= zrot;
      points[2] -= zrot;
      points[3] -= zrot;
      points[4] -= zrot;
      points[5] -= zrot;
      zrot -= 0.3;
    }
  }

  //!GLFW mouse click callback
  void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
  {
      if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        std::cout << "Left clicked me at " << xpos << ", " << ypos << std::endl;        
      }
  }
};  
  


