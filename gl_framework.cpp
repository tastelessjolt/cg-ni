#include "gl_framework.hpp"

extern GLfloat xrot;
extern GLfloat yrot;
extern GLfloat zrot;

extern GLfloat xpos;
extern GLfloat ypos;
extern GLfloat zpos;

extern std::vector<float> points;
extern std::vector<float> triangles;

extern int mode;

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
      yrot-=0.2;
    } 
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      yrot+=0.2;
    }
    else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      xrot-=0.2;
    }
    else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      xrot+=0.2;
    }
    else if (key == GLFW_KEY_PAGE_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      zrot-=0.2;
    }
    else if (key == GLFW_KEY_PAGE_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      zrot+=0.2;
    }
    else if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      ypos += 0.05;
    } 
    else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      xpos -= 0.05;
      
    }
    else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      ypos -= 0.05;
      
    }
    else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      xpos += 0.05;
      
    }
    else if (key == GLFW_KEY_Z && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      zpos -= 0.05;
      
    }
    else if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      zpos += 0.05;
    }
    else if (key == GLFW_KEY_C && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      
      for (int i = 0; i < ((int)points.size()-6); i += 3){
        triangles.push_back(points[i]);
        triangles.push_back(points[i+1]);
        triangles.push_back(points[i+2]);
        triangles.push_back(points[i+3]);
        triangles.push_back(points[i+4]);
        triangles.push_back(points[i+5]);
        triangles.push_back(points[i+6]);
        triangles.push_back(points[i+7]);
        triangles.push_back(points[i+8]);
      }

      points.clear();

    }
    else if (key == GLFW_KEY_M && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      mode = CI_MODELLING_MODE;
    }
    else if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      mode = CI_INSPECTION_MODE;
    }
    else if (key == GLFW_KEY_R && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      int sumx = 0, sumy = 0, sumz = 0;
      for (int i = 0; i < triangles.size()/9; ++i){
        sumx += triangles[9*i];
        sumy += triangles[9*i+1];
        sumz += triangles[9*i+2];
      }

      int p = triangles.size()/9;
      sumx += triangles[9*p + 3] + triangles[9*p + 6];
      sumy += triangles[9*p + 4] + triangles[9*p + 7];
      sumz += triangles[9*p + 5] + triangles[9*p + 8];

      xpos = sumx/(p + 2.0);
      ypos = sumy/(p + 2.0);
      zpos = sumz/(p + 2.0);
    }
  }

  void convert_to_world(GLFWwindow* window, GLint x, GLint y, GLfloat* xf, GLfloat* yf){
    
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // ensure camera look at vector is normal to xy plane whenever using this fn
    *xf = -1 * (x - width/2.0)/(width/2.0) + xpos;
    *yf = -1 * (y - height/2.0)/(height/2.0) + ypos;

  }
  //!GLFW mouse click callback
  void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
  {
      if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mode == CI_MODELLING_MODE){
        
        if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)){

          if (points.size() >= 3){
            points.pop_back();
            points.pop_back();
            points.pop_back();
          }

        } else {

          double xclk, yclk;
          glfwGetCursorPos(window, &xclk, &yclk);

          float x_in, y_in;
          convert_to_world(window, xclk, yclk, &x_in, &y_in);
          std::cout << "Left click: (" << x_in << "," << y_in <<  ")" << std::endl;

          points.push_back(x_in);
          points.push_back(y_in);
          points.push_back(zpos);
        
        }


      }
  }

};  
  


