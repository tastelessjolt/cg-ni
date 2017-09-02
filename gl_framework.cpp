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
extern glm::vec3 getCentroid();

std::vector<float> last_tr;

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

  //!save the model to file 
  void save_to_file() {
    std::string filename;
    std::cout << "Enter the name of the file (without extension): "; std::cin >> filename;

    std::fstream fs(filename + ".raw", std::fstream::out);

    for (int i = 0; i < triangles.size()/6; ++i){
      fs << triangles[i*6] << " " << triangles[i*6+1] << " " << triangles[i*6+2] << " " << triangles[i*6+3] << " " << triangles[i*6+4] << " " << triangles[i*6+5] << std::endl;
    }

    fs.close();
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


  void quit (GLFWwindow *window) {
    char c;
    std::cout << "Do you want to save your model? (y/n)" << std:: endl;
    std::cin >> c;
    if (! (c == 'n' || c == 'N') ){
      save_to_file();
    }
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    //!Close the window if the ESC key was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      quit(window);
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

      if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)){
        if (triangles.size() >= 3 * 6){
          // popping the last points added for a triangle
          for (int i = 0; i < 3 * 6; ++i){
            last_tr.push_back(triangles.back());
            triangles.pop_back();
          }
          std::cout << "Cleared the last drawn triangle" << std::endl;
        }
      }else{
        // move z plane
        zpos -= 0.05; 
      }      
    }
    else if (key == GLFW_KEY_Y && (action == GLFW_PRESS)) {

      if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)){
        if (last_tr.size() >= 3 * 6) { 
          for (int i = 0; i < 3 * 6; ++i){
            triangles.push_back(last_tr.back());
            last_tr.pop_back();
          }
          std::cout << "Re-drew the last cleared triangle" << std::endl;
        }
      } 
    }
    else if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      zpos += 0.05;
    }
    else if (key == GLFW_KEY_C && (action == GLFW_PRESS)) {
      
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
      std::cout << "Enter color in the `R G B` format in float: ";
      std::cin >> rcol >> gcol >> bcol;
    }
    else if (key == GLFW_KEY_M && (action == GLFW_PRESS)) {
      mode = CI_MODELLING_MODE;
    }
    else if (key == GLFW_KEY_I && (action == GLFW_PRESS)) {
      mode = CI_INSPECTION_MODE;
    }
    else if (key == GLFW_KEY_R && (action == GLFW_PRESS)) {
      glm::vec3 pos = getCentroid();
      xpos = pos[0];
      ypos = pos[1];
      zpos = pos[2];
    }
    else if (key == GLFW_KEY_K && (action == GLFW_PRESS)) {
      save_to_file();
    }
    else if (key == GLFW_KEY_L && (action == GLFW_PRESS)) {
      std::string filename;
      std::cout << "Enter the name of the file to be loaded (without extension): "; std::cin >> filename;
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
    }
    else if (key == GLFW_KEY_F && (action == GLFW_PRESS)) {
      std::string scenefilename;
      std::cout << "Enter the name of the scene file to be loaded (without extension scn): "; std::cin >> scenefilename;
      scenefilename+=".scn";

      struct stat buffer;
      if (stat(scenefilename.c_str(), &buffer) != 0){
        std::cerr << "Error: File not found" << std::endl;
        return;
      }

      std::fstream fs(scenefilename, std::fstream::in);

      std::vector<GLfloat> scenetriangles[3];
      std::vector<GLfloat> scaleParams[3];
      std::vector<GLfloat> rotationParams[3];
      std::vector<GLfloat> translationParams[3];

      GLfloat eye[3];
      GLfloat lookat[3];
      GLfloat up[3];

      GLfloat frustum[6];

      triangles.clear();
      points.clear();
      
      int numfile = 3;
      int nfile = 0;
      int nline = 0;

      float tmp;
      std::string line;
      while(!fs.eof()){
        std::getline(fs, line);
        std::cout << "nline: " << nline << std::endl;
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
                for(int i = 0; i < 3; i++){
                  iss >> frustum[i];
                }
                break;
              case 4:
                for(int i = 3; i < 5; i++){
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
      if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mode == CI_MODELLING_MODE){
        
        if ((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)){

          if (points.size() >= 6){
            int len = points.size();
            std::cout << "Removed point (" << points[len - 6] << ", " << points[len - 5] << ", " << points[len - 4] << ") (" \
              << points[len - 3] << ", " << points[len - 2] << ", " << points[len - 1] << ")" << std::endl;  
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
          std::cout << "Point at click: (" << x_in << "," << y_in << "," << zpos << ") ";
          std::cout << "(" << rcol << "," << gcol << "," << bcol << ")" << std::endl;

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
  


