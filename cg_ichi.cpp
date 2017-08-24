#include "gl_framework.hpp"
#include "shader_util.hpp"

#include <unistd.h>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

std::vector<float> points;
std::vector<float> triangles;

int mode = CI_MODELLING_MODE;

GLuint shaderProgram;
GLuint vbo_points, vbo_triangles, vao_points, vao_triangles;

glm::mat4 rotation_matrix;
glm::mat4 ortho_matrix;
glm::mat4 modelview_matrix;
glm::mat4 look_at;
glm::mat4 translate;
glm::mat4 translate_inv;
GLuint uModelViewMatrix;

GLfloat xrot = 0;
GLfloat yrot = 0;
GLfloat zrot = 0;

GLfloat xpos = 0;
GLfloat ypos = 0;
GLfloat zpos = 0;

GLfloat rcol = 0.0f;
GLfloat gcol = 0.0f;
GLfloat bcol = 0.0f;

int mode_state;
bool mode_printed = false;

GLfloat zpos_state;
bool zpos_printed = false;

GLfloat rcol_state, gcol_state, bcol_state;
bool col_printed = false;

glm::vec3 getCentroid () {
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

  return glm::vec3(sumx/(p + 2.0), sumy/(p + 2.0), sumz/(p + 2.0));
}

void printState()
{
  if ( mode_state != mode ){
    mode_printed = false;
    mode_state = mode;
  }
  if ( zpos_state != zpos ){
    zpos_printed = false;
    zpos_state = zpos;
  }
  if ( rcol_state != rcol or gcol_state != gcol or bcol_state != bcol ){
    col_printed = false;
    rcol_state = rcol;
    gcol_state = gcol;
    bcol_state = bcol;
  }
    
  if (!mode_printed){
    std::cout << "Mode: " << ((mode == CI_MODELLING_MODE)?"Modelling":"Inspection") << std::endl;
    mode_printed = true;
  }
  if (!zpos_printed){
    std::cout << "Drawing plane at: " << zpos << std::endl;
    zpos_printed = true;
  }
  if (!col_printed){
    std::cout << "Current color: " << "(" << rcol << ", " << gcol << ", " << bcol << ")" << std::endl;
    col_printed = true;
  }
}

void initShadersGL(void)
{
  std::string vertex_shader_file("simple_vs.glsl");
  std::string fragment_shader_file("simple_fs.glsl");

  std::vector<GLuint> shaderList;
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = csX75::CreateProgramGL(shaderList);
  
}

void initVertexBufferGL(void)
{
  //Ask GL for a Vertex Buffer Object (vbo)
  glGenBuffers (1, &vbo_points);

  // Set it as the current buffer to be used by binding it
  // glBindBuffer (GL_ARRAY_BUFFER, vbo_points);
  //Copy the points into the current buffer - 9 float values, start pointer and static data
  // glBufferData (GL_ARRAY_BUFFER, points.size() * sizeof (float), &points[0], GL_DYNAMIC_DRAW);

  glGenBuffers (1, &vbo_triangles);
  //   // Set it as the current buffer to be used by binding it
  // glBindBuffer (GL_ARRAY_BUFFER, vbo_triangles);
  // //Copy the points into the current buffer - 9 float values, start pointer and static data
  // glBufferData (GL_ARRAY_BUFFER, triangles.size() * sizeof (float), &triangles[0], GL_DYNAMIC_DRAW);

  //Ask GL for a Vertex Attribute Object (vao_points)
  glGenVertexArrays (1, &vao_points);
  //Set it as the current array to be used by binding it
  // glBindVertexArray (vao_points);
  //Enable the vertex attribute
  // glEnableVertexAttribArray (0);
  //This the layout of our first vertex buffer
  //"0" means define the layout for attribute number 0. "3" means that the variables are vec3 made from every 3 floats 
  // glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  uModelViewMatrix = glGetUniformLocation(shaderProgram, "uModelViewMatrix");

  glPointSize(5.0f);
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shaderProgram);
  
  if (mode == CI_MODELLING_MODE){
    look_at = glm::lookAt(glm::vec3(xpos, ypos, -2.0), glm::vec3(xpos, ypos, 1.0), glm::vec3(0.0, 1.0, 0.0));
  } 
  else {
    glm::vec3 centroid = getCentroid();

    translate = glm::translate(glm::mat4(1.0f), -centroid);
    translate_inv = glm::translate(glm::mat4(1.0f), centroid);

    rotation_matrix = glm::rotate(glm::mat4(1.0f), xrot, glm::vec3(1.0f,0.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, yrot, glm::vec3(0.0f,1.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, zrot, glm::vec3(0.0f,0.0f,1.0f));
    
    look_at = glm::lookAt(glm::vec3(xpos, ypos, -2.0), glm::vec3(xpos, ypos, 1.0), glm::vec3(0.0, 1.0, 0.0));
    look_at = (look_at * (translate_inv * rotation_matrix * translate));
  }
  
  ortho_matrix = glm::ortho(-1.0, 1.0, -1.0, 1.0, -10.0, 10.0);
  modelview_matrix = ortho_matrix * look_at;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  
  GLuint vp = glGetAttribLocation( shaderProgram, "vp");
  GLuint vcol = glGetAttribLocation( shaderProgram, "vcol");
  

  glBindBuffer (GL_ARRAY_BUFFER, vbo_points);
  glBufferData (GL_ARRAY_BUFFER, points.size() * sizeof (float), &points[0], GL_DYNAMIC_DRAW);
  glBindVertexArray (vao_points);

  glEnableVertexAttribArray (vp);
  glEnableVertexAttribArray (vcol);

  glVertexAttribPointer (vp, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), NULL);
  glVertexAttribPointer (vcol, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));

  glDrawArrays(GL_POINTS, 0, points.size()/6);


  glBindBuffer (GL_ARRAY_BUFFER, vbo_triangles);
  glBufferData (GL_ARRAY_BUFFER, triangles.size() * sizeof (float), &triangles[0], GL_DYNAMIC_DRAW);
  glBindVertexArray (vao_points);

  glEnableVertexAttribArray (vp);
  glEnableVertexAttribArray (vcol);

  glVertexAttribPointer (vp, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), NULL);
  glVertexAttribPointer (vcol, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));

  glDrawArrays(GL_TRIANGLES, 0, triangles.size()/6);

}

int main(int argc, char** argv)
{
  //! The pointer to the GLFW window
  GLFWwindow* window;

  //! Setting up the GLFW Error callback
  glfwSetErrorCallback(csX75::error_callback);

  //! Initialize GLFW
  if (!glfwInit())
    return -1;

  //We want OpenGL 4.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  //This is for MacOSX - can be omitted otherwise
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
  //We don't want the old OpenGL 
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

  //! Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(640, 480, "CS475/CS675 OpenGL Framework", NULL, NULL);
  if (!window)
    {
      glfwTerminate();
      return -1;
    }
  
  //! Make the window's context current 
  glfwMakeContextCurrent(window);

  //Initialize GLEW
  //Turn this on to get Shader based OpenGL
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
      //Problem: glewInit failed, something is seriously wrong.
      std::cerr<<"GLEW Init Failed : %s"<<std::endl;
    }

  //Print and see what context got enabled
  std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
  std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
  std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
  std::cout<<"GLSL Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;

  //Keyboard Callback
  glfwSetKeyCallback(window, csX75::key_callback);
  //Framebuffer resize callback
  glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

  // Ensure we can capture the escape key being pressed below
  // glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  glfwSetMouseButtonCallback(window, csX75::mouse_button_callback);

  //Initialize GL state
  csX75::initGL();
  initShadersGL();
  initVertexBufferGL();

  // Loop until the user closes the window
  while (glfwWindowShouldClose(window) == 0)
    {
      printState();

      // Render here
      renderGL();

      // Swap front and back buffers
      glfwSwapBuffers(window);
      
      // Poll for and process events
      glfwPollEvents();
    }
  
  glfwTerminate();
  return 0;
}

