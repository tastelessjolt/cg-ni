#include "gl_framework.hpp"
#include <unistd.h>
#include <sys/stat.h>

extern GLfloat xrot;
extern GLfloat yrot;
extern GLfloat zrot;

extern GLfloat xpos;
extern GLfloat ypos;
extern GLfloat zpos;

extern GLfloat rcol;
extern GLfloat gcol;
extern GLfloat bcol;

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
      yrot-=0.2*((mode == CI_INSPECTION_MODE)?1:0);
    } 
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      yrot+=0.2*((mode == CI_INSPECTION_MODE)?1:0);
    }
    else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      xrot-=0.2*((mode == CI_INSPECTION_MODE)?1:0);
    }
    else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      xrot+=0.2*((mode == CI_INSPECTION_MODE)?1:0);
    }
    else if (key == GLFW_KEY_PAGE_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      zrot-=0.2*((mode == CI_INSPECTION_MODE)?1:0);
    }
    else if (key == GLFW_KEY_PAGE_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      zrot+=0.2*((mode == CI_INSPECTION_MODE)?1:0);
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
    else if (key == GLFW_KEY_C && (action == GLFW_PRESS)) {
      
      // jump by one vertex
      for (int i = 0; i < ((int)points.size()-12); i += 6){
        
        for (int j = 0; j < 3 * 6; ++j){
          triangles.push_back(points[i + j]);
        }

      }

      points.clear();

    }
    // color handling
    // setting current color
    else if (key == GLFW_KEY_U && (action == GLFW_PRESS)) {
      std::cin >> rcol >> gcol >> bcol;
    }
    else if (key == GLFW_KEY_M && (action == GLFW_PRESS)) {
      mode = CI_MODELLING_MODE;
    }
    else if (key == GLFW_KEY_I && (action == GLFW_PRESS)) {
      mode = CI_INSPECTION_MODE;
    }
    else if (key == GLFW_KEY_R && (action == GLFW_PRESS)) {
      
      GLfloat sumx = 0, sumy = 0, sumz = 0;
      for (int i = 0; i < triangles.size()/18; ++i){
        sumx += triangles[18*i];
        sumy += triangles[18*i+1];
        sumz += triangles[18*i+2];
      }

      int p = triangles.size()/18;
      sumx += triangles[18*(p-1) + 0 + 6] + triangles[18*(p-1) + 0 + 12];
      sumy += triangles[18*(p-1) + 1 + 6] + triangles[18*(p-1) + 1 + 12];
      sumz += triangles[18*(p-1) + 2 + 6] + triangles[18*(p-1) + 2 + 12];

      xpos = sumx/(p + 2.0);
      ypos = sumy/(p + 2.0);
      zpos = sumz/(p + 2.0);

      // std::cout << sumx << " " << sumy << " " << sumz << " " << p << std::endl;
      // std::cout << xpos << " " << ypos << " " << zpos << std::endl;
    }
    else if (key == GLFW_KEY_K && (action == GLFW_PRESS)) {
      
      std::string filename;
      std::cin >> filename;

      std::fstream fs(filename + ".raw", std::fstream::out);

      for (int i = 0; i < triangles.size()/6; ++i){
        fs << triangles[i*6] << " " << triangles[i*6+1] << " " << triangles[i*6+2] << " " << triangles[i*6+3] << " " << triangles[i*6+4] << " " << triangles[i*6+5] << std::endl;
      }

      fs.close();

    }
    else if (key == GLFW_KEY_L && (action == GLFW_PRESS)) {
      
      std::string filename;
      std::cin >> filename;
      filename+=".raw";

      struct stat buffer;
      if (stat(filename.c_str(), &buffer) != 0){
        std::cerr << "Error: File not found" << std::endl;
        return;
      }

      std::fstream fs(filename, std::fstream::in);

      triangles.clear();
      points.clear();
      
      float tmp;
      while(!fs.eof()){
        fs >> tmp;
        triangles.push_back(tmp);
      }

      fs.close();

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

          if (points.size() >= 6){
            points.pop_back();
            points.pop_back();
            points.pop_back();
            points.pop_back();
            points.pop_back();
            points.pop_back();
          }

        } else {

          double xclk, yclk;
          glfwGetCursorPos(window, &xclk, &yclk);

          float x_in, y_in;
          convert_to_world(window, xclk, yclk, &x_in, &y_in);
          std::cout << "Left click: (" << x_in << "," << y_in << "," << zpos << ")" << std::endl;
          std::cout << "Colour: (" << rcol << "," << gcol << "," << bcol << ")" << std::endl;

          points.push_back(x_in);
          points.push_back(y_in);
          points.push_back(zpos);
          points.push_back(rcol);
          points.push_back(gcol);
          points.push_back(bcol);
        
        }


      }
  }

};  
  


