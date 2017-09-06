#include "gl_framework.hpp"
#include "shader_util.hpp"

#include <unistd.h>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

GLuint shaderProgram;

GLuint vao;
GLuint frame;
GLuint * vbo_vec;
GLuint vp, vcol;

std::vector<GLfloat> scenetriangles[N_OBJECTS];
std::vector<GLfloat> scaleParams[N_OBJECTS];
std::vector<GLfloat> rotationParams[N_OBJECTS];
std::vector<GLfloat> translationParams[N_OBJECTS];

GLfloat eye[3];
GLfloat lookat[3];
GLfloat up[3];

GLfloat frustum[6];

glm::mat4 rotation_matrix;
glm::mat4 ortho_matrix;
glm::mat4 modelview_matrix;
glm::mat4 look_at, look_at_inv;

glm::mat4 translate;

GLuint uModelViewMatrix;

GLfloat xrot = 0;
GLfloat yrot = 0;
GLfloat zrot = 0;

GLfloat xpos = 0;
GLfloat ypos = 0;
GLfloat zpos = 0;

glm::vec3 getCentroid (std::vector<GLfloat> object) {
  if (object.size() > 18) {   
    GLfloat sumx = 0, sumy = 0, sumz = 0;
    for (int i = 0; i < object.size()/18; ++i){
      sumx += object[18*i];
      sumy += object[18*i+1];
      sumz += object[18*i+2];
    }

    int p = object.size()/18;
    sumx += object[18*(p-1) + 0 + 6] + object[18*(p-1) + 0 + 12];
    sumy += object[18*(p-1) + 1 + 6] + object[18*(p-1) + 1 + 12];
    sumz += object[18*(p-1) + 2 + 6] + object[18*(p-1) + 2 + 12];

    return glm::vec3(sumx/(p + 2.0), sumy/(p + 2.0), sumz/(p + 2.0));
  }
  return glm::vec3(0, 0, 0);
}

void printState()
{
  // TODO
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
  vbo_vec = new GLuint[N_OBJECTS];
  glGenBuffers (N_OBJECTS, vbo_vec);

  glGenBuffers (1, &frame);

  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (1, &vao);

  uModelViewMatrix = glGetUniformLocation(shaderProgram, "uModelViewMatrix");

  vp = glGetAttribLocation( shaderProgram, "vp");
  vcol = glGetAttribLocation( shaderProgram, "vcol");

  glPointSize(5.0f);
}

void pushPoint(std::vector<GLfloat> &frame_lines, GLfloat x, GLfloat y, GLfloat z) {
  frame_lines.push_back(x);
  frame_lines.push_back(y);
  frame_lines.push_back(z);
}

void createFrameLines(std::vector<GLfloat> &frame_lines, GLfloat L, GLfloat R, GLfloat T, GLfloat B, GLfloat N, GLfloat F) { 
  // Eye to NEAR
  pushPoint(frame_lines, 0.0, 0.0, 0.0);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);
  pushPoint(frame_lines, R, T, -N);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);


  pushPoint(frame_lines, 0.0, 0.0, 0.0);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);
  pushPoint(frame_lines, -L, T, -N);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);


  pushPoint(frame_lines, 0.0, 0.0, 0.0);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);
  pushPoint(frame_lines, R, -B, -N);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);


  pushPoint(frame_lines, 0.0, 0.0, 0.0);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);
  pushPoint(frame_lines, -L, -B, -N);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);


  // NEAR to FAR 
  pushPoint(frame_lines, R * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, -L * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, -L, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, R * F/N, -B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R, -B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, -L * F/N, -B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, -L, -B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  // NEAR Plane
  pushPoint(frame_lines, -L, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, R, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R, -B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, R, -B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, -L, -B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, -L, -B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, -L, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  // FAR Plane
  pushPoint(frame_lines, -L * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, R * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R * F/N, -B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, R * F/N, -B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, -L * F/N, -B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, -L * F/N, -B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, -L * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


}


void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shaderProgram);

  glm::mat4 sceneTranform = glm::ortho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
  
  
  // Lines / Projectors 
  look_at = glm::lookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(lookat[0], lookat[1], lookat[2]), glm::vec3(up[0], up[1], up[2]));
  look_at_inv = glm::inverse(look_at);

  std::vector<GLfloat> frame_lines;
  createFrameLines(frame_lines, frustum[0], frustum[1], frustum[2], frustum[3], frustum[4], frustum[5]);

  modelview_matrix = sceneTranform * look_at_inv;

  // Make shader variable mappings 
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));

  // Bind Buffer object
  glBindBuffer (GL_ARRAY_BUFFER, frame);
  glBufferData (GL_ARRAY_BUFFER, frame_lines.size() * sizeof (float), &(frame_lines[0]), GL_DYNAMIC_DRAW);
  // Bind Array object
  glBindVertexArray (vao);

  glEnableVertexAttribArray (vp);
  glEnableVertexAttribArray (vcol);

  glVertexAttribPointer (vp, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), NULL);
  glVertexAttribPointer (vcol, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));

  // Call glDraw
  glDrawArrays(GL_LINES, 0, frame_lines.size()/6);



  // Generate scene matrix 
  for (int i = 0; i != N_OBJECTS; i++) {
    // Generate matrix for each object 
    glm::vec3 centroid = getCentroid(scenetriangles[i]);

    glm::mat4 translate_centering = glm::translate(glm::mat4(1.0f), -centroid);
    // translate_centering_inv = glm::translate(glm::mat4(1.0f), centroid);

    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(translationParams[i][0], translationParams[i][1], translationParams[i][2]));
    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(scaleParams[i][0], scaleParams[i][1], scaleParams[i][2]));

    glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), rotationParams[i][0], glm::vec3(1.0f,0.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, rotationParams[i][1], glm::vec3(0.0f,1.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, rotationParams[i][2], glm::vec3(0.0f,0.0f,1.0f));

    glm::mat4 objectViewTranform = translate * rotation_matrix * scaling * translate_centering;
    modelview_matrix = sceneTranform * objectViewTranform;

    // Make shader variable mappings 
    glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));

    // Bind Buffer object
    glBindBuffer (GL_ARRAY_BUFFER, vbo_vec[i]);
    glBufferData (GL_ARRAY_BUFFER, scenetriangles[i].size() * sizeof (float), &(scenetriangles[i][0]), GL_DYNAMIC_DRAW);
    // Bind Array object
    glBindVertexArray (vao);

    glEnableVertexAttribArray (vp);
    glEnableVertexAttribArray (vcol);

    glVertexAttribPointer (vp, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), NULL);
    glVertexAttribPointer (vcol, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));

    // Call glDraw
    glDrawArrays(GL_TRIANGLES, 0, scenetriangles[i].size()/6);
  }
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
  csX75::load_scene_file();
  
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

