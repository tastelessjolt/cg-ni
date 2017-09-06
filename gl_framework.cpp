#include "gl_framework.hpp"
#include <unistd.h>
#include <sys/stat.h>

extern glm::vec3 getCentroid(std::vector<GLfloat>);

extern GLfloat xrot;
extern GLfloat yrot;
extern GLfloat zrot;

extern GLfloat xpos;
extern GLfloat ypos;
extern GLfloat zpos;

extern std::vector<GLfloat> scenetriangles[N_OBJECTS];
extern std::vector<GLfloat> scaleParams[N_OBJECTS];
extern std::vector<GLfloat> rotationParams[N_OBJECTS];
extern std::vector<GLfloat> translationParams[N_OBJECTS];

extern GLfloat eye[3];
extern GLfloat lookat[3];
extern GLfloat up[3];

extern GLfloat frustum[6];

namespace csX75
{
  //! Initialize GL State
  void initGL(void)
  {
    //Set framebuffer clear color
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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

  std::vector<GLfloat> readrawfile (std::string filename) {
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) != 0){
      std::cerr << "Error: File not found" << std::endl;
      std::vector<GLfloat> v;
      return v;
    }

    std::fstream fs(filename, std::fstream::in);

    std::vector<GLfloat> triangles;
    
    float tmp;
    std::string line;
    while(!fs.eof()){
      std::getline(fs, line);
      if (line != ""){
        std::stringstream iss(line);
        for(int i = 0; i < 6; i++){
          iss >> tmp;
          triangles.push_back(tmp);
        }
      }
    }

    fs.close();

    return triangles;
  }

  void load_scene_file() {

    std::string scenefilename = "myscene.scn";
    struct stat buffer;
    if (stat(scenefilename.c_str(), &buffer) != 0){
      std::cout << "Enter the name of the scene file to be loaded (without extension scn): "; std::cin >> scenefilename;
      scenefilename+=".scn";

      if (stat(scenefilename.c_str(), &buffer) != 0){
        std::cerr << "Error: File not found" << std::endl;
        return;
      }
    }
    std::fstream fs(scenefilename, std::fstream::in);

    for (int i = 0; i != N_OBJECTS; i++) {
      scenetriangles[i].clear();
      scaleParams[i].clear();
      rotationParams[i].clear();
      translationParams[i].clear();
    }
    
    int numfile = N_OBJECTS;
    int nfile = 0;
    int nline = 0;

    float tmp;
    std::string line;
    while(!fs.eof()){
      std::getline(fs, line);
      if (line != "" && line[0] != '#'){
        if (nfile < numfile) {
          if (nline == 0) {
            std::cout << line << std::endl;
            scenetriangles[nfile] = readrawfile(line);
          }
          else {
            std::stringstream iss(line);
            for(int i = 0; i < 3; i++){
              iss >> tmp;
              switch (nline) {
                case 1:
                  scaleParams[nfile].push_back(tmp);
                  break;
                case 2:
                  rotationParams[nfile].push_back(tmp);
                  break;
                case 3:
                  translationParams[nfile].push_back(tmp);
                  break;
              }
            }
          }
          nline ++;

          if (nline == 4) { 
            nfile ++;
            nline = 0;
          }
        }
        else {
          std::stringstream iss(line);
          switch (nline) {
            case 0:
              for(int i = 0; i < 3; i++){
                iss >> eye[i];
              }
              break;
            case 1:
              for(int i = 0; i < 3; i++){
                iss >> lookat[i];
              }
              break;
            case 2:
              for(int i = 0; i < 3; i++){
                iss >> up[i];
              }
              break;
            case 3:
              for(int i = 0; i < 4; i++){
                iss >> frustum[i];
              }
              break;
            case 4:
              for(int i = 4; i < 6; i++){
                iss >> frustum[i];
              }
              break;
          }
          nline ++;
        }
      }
    }
    fs.close();
    
    // for (int i = 0; i != numfile; i++ ) {
    //   for (float coord : scenetriangles[i]) {
    //     std::cout << coord << " ";
    //   }
    //   std::cout << std::endl;
    //   std::cout << std::endl;
    //   std::cout << std::endl;
    // }

    // for (int i = 0; i != 3; i++) {
    //   std::cout << eye[i] << " ";
    // }
    // std::cout << std::endl;

    // for (int i = 0; i != 3; i++) {
    //   std::cout << lookat[i] << " ";
    // }
    // std::cout << std::endl;
    
    // for (int i = 0; i != 3; i++) {
    //   std::cout << up[i] << " ";
    // }
    // std::cout << std::endl;
    
    // for (int i = 0; i != 5; i++) {
    //   std::cout << frustum[i] << " ";
    // }
    // std::cout << std::endl;
  }

  void quit (GLFWwindow *window) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    //!Close the window if the ESC key was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      quit(window);
    else if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      yrot -= 0.2;
    } 
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      yrot += 0.2;
    }
    else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      xrot -= 0.2;
    }
    else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      xrot += 0.2;
    }
    else if (key == GLFW_KEY_PAGE_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      zrot -= 0.2;
    }
    else if (key == GLFW_KEY_PAGE_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      zrot += 0.2;
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
      // TODO Zoom in/out
      zpos -= 0.05; 
    }
    else if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      zpos += 0.05;
    }
    else if (key == GLFW_KEY_F && (action == GLFW_PRESS)) {
      load_scene_file();
    }
    else if (key == GLFW_KEY_Q && (action == GLFW_PRESS)) {
      quit(window);
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
      
  }

};  
  


