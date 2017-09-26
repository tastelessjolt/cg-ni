#include "gl_framework.hpp"
#include "shader_util.hpp"

#include <unistd.h>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

//! The pointer to the GLFW window
GLFWwindow* window;

GLuint shaderProgram;

GLuint * vao_vec, vao_frame;
GLuint frame;
GLuint * vbo_vec;
GLuint vp, vcol;

std::vector<GLfloat> scenetriangles[N_OBJECTS];
std::vector<GLfloat> scaleParams[N_OBJECTS];
std::vector<GLfloat> rotationParams[N_OBJECTS];
std::vector<GLfloat> translationParams[N_OBJECTS];
std::vector<GLfloat> frame_lines;

GLfloat eye[3];
GLfloat lookat[3];
GLfloat up[3];

GLfloat frustum[6];
GLfloat R, L, T, B, N, F;

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

view currView = WCS;

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
  pushPoint(frame_lines, L, T, -N);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);


  pushPoint(frame_lines, 0.0, 0.0, 0.0);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);
  pushPoint(frame_lines, R, B, -N);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);


  pushPoint(frame_lines, 0.0, 0.0, 0.0);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);
  pushPoint(frame_lines, L, B, -N);
  pushPoint(frame_lines, 1.0, 0.0, 0.0);


  // NEAR to FAR 
  pushPoint(frame_lines, R * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, L * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, L, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, R * F/N, B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R, B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, L * F/N, B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, L, B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  // NEAR Plane
  pushPoint(frame_lines, L, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, R, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R, B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, R, B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, L, B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, L, B, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, L, T, -N);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  // FAR Plane
  pushPoint(frame_lines, L * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, R * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, R * F/N, B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, R * F/N, B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, L * F/N, B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);


  pushPoint(frame_lines, L * F/N, B * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
  pushPoint(frame_lines, L * F/N, T * F/N, -F);
  pushPoint(frame_lines, 0.0, 1.0, 1.0);
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

  L = frustum[0];
  R = frustum[1];
  T = frustum[2];
  B = frustum[3];
  N = frustum[4];
  F = frustum[5];
  //Ask GL for a Vertex Buffer Object (vbo)
  vbo_vec = new GLuint[N_OBJECTS];
  vao_vec = new GLuint[N_OBJECTS];

  glGenBuffers (N_OBJECTS, vbo_vec);
  glGenBuffers (1, &frame);

  //Ask GL for a Vertex Attribute Object (vao)
  glGenVertexArrays (N_OBJECTS, vao_vec);
  glGenVertexArrays (1, &vao_frame);

  createFrameLines(frame_lines, frustum[0], frustum[1], frustum[2], frustum[3], frustum[4], frustum[5]);

  vp = glGetAttribLocation( shaderProgram, "vp");
  vcol = glGetAttribLocation( shaderProgram, "vcol");

  uModelViewMatrix = glGetUniformLocation(shaderProgram, "uModelViewMatrix");
  
  // Frustum VAO_vec Related 
  // Bind Array object
  glBindVertexArray (vao_frame);

  // Bind Buffer object
  glBindBuffer (GL_ARRAY_BUFFER, frame);
  glBufferData (GL_ARRAY_BUFFER, frame_lines.size() * sizeof (float), &(frame_lines[0]), GL_STATIC_DRAW);

  glEnableVertexAttribArray (vp);
  glEnableVertexAttribArray (vcol);

  glVertexAttribPointer (vp, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), NULL);
  glVertexAttribPointer (vcol, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));


  // Objects VAO_vec Related 
  for (int i = 0; i != N_OBJECTS; i++) {
    // Bind Array object
    glBindVertexArray (vao_vec[i]);

    glEnableVertexAttribArray (vp);
    glEnableVertexAttribArray (vcol);

    // Bind Buffer object
    glBindBuffer (GL_ARRAY_BUFFER, vbo_vec[i]);
    glBufferData (GL_ARRAY_BUFFER, scenetriangles[i].size() * sizeof (float), &(scenetriangles[i][0]), GL_STATIC_DRAW);
    glVertexAttribPointer (vp, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), NULL);
    glVertexAttribPointer (vcol, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
  }

  glPointSize(5.0f);
}

glm::mat4 make_frustum(GLfloat R, GLfloat L, GLfloat B, GLfloat T, GLfloat N, GLfloat F) {
  GLfloat arr[] =  { 
    2 * N / (L - R),      0,                0,                      0,
    0,                    2 * N / (T - B),  0,                      0,
    (R + L)/(R - L),      (T + B)/(T - B),  - (F + N) / (F - N),    -1,
    0,                    0,                -2 * F * N /(F - N),    0
  };
  return glm::make_mat4(arr);
}

glm::mat4 make_lookAt(glm::vec3 eye, glm::vec3 lookat, glm::vec3 up) {
  glm::vec3 n = glm::normalize(-(lookat - eye));  
  glm::vec3 u = glm::normalize(glm::cross(up, n));
  glm::vec3 v = glm::cross(n, u);

  glm::vec3 eye_ = -glm::transpose(glm::mat3(u, v, n)) * eye;

  return glm::transpose(glm::mat4 (glm::vec4(u, eye_[0]), glm::vec4(v, eye_[1]), glm::vec4(n, eye_[2]), glm::vec4(0.0, 0.0, 0.0, 1.0)));
}

glm::mat4 make_ndcs2dcs(GLfloat l, GLfloat r, GLfloat b, GLfloat t) {
  return glm::mat4(
    glm::vec4((r - l)/2,    0.0,            0.0,      0.0), 
    glm::vec4(0.0,          (t - b)/2,      0.0,      0.0), 
    glm::vec4(0.0,          0.0,            0.01,      0.0), 
    glm::vec4((r + l)/2,    (t + b)/2,      0.01,      1.0)
    );
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shaderProgram);

  glm::mat4 world_look_at = make_lookAt(glm::vec3(xpos, ypos, 1.0), glm::vec3(xpos, ypos, -1.0), glm::vec3(0.0, 1.0, 0.0));

  glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), xrot, glm::vec3(1.0f,0.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, yrot, glm::vec3(0.0f,1.0f,0.0f));
  rotation_matrix = glm::rotate(rotation_matrix, zrot, glm::vec3(0.0f,0.0f,1.0f));

  world_look_at = world_look_at * rotation_matrix;

  // Lines / Projectors 
  look_at = make_lookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(lookat[0], lookat[1], lookat[2]), glm::vec3(up[0], up[1], up[2]));

  glm::mat4 secondary_ortho = ((glm::mat4)glm::ortho(-2.0 - zpos, 2.0 + zpos, -2.0 - zpos, 2.0 + zpos, -100.0, 100.0)) * world_look_at;
  glm::mat4 sceneTranform;
  switch(currView) {
    case WCS:
      sceneTranform = secondary_ortho;
    break;
    case VCS:
      sceneTranform = secondary_ortho * look_at ;
    break;
    case CCS:
      sceneTranform = secondary_ortho * ((glm::mat4)make_frustum(R, L, B, T, N, F)) * look_at;
    break;
    case NDCS:
      sceneTranform = secondary_ortho * ((glm::mat4)make_frustum(R, L, B, T, N, F)) * look_at;
    break;
    case DCS:
      int width, height;
      glfwGetWindowSize(window, &width, &height);
      GLfloat l = 0, b = 0;
      GLfloat r = width, t = height;
      sceneTranform = secondary_ortho * ((glm::mat4)glm::ortho((double)l, (double)r, (double)b, (double)t, -1.0, 1.0)) * ((glm::mat4)make_ndcs2dcs(l, r, b, t)) * ((glm::mat4)make_frustum(R, L, B, T, N, F)) * look_at;
    break; 
  }
  
  

  look_at_inv = glm::inverse(look_at);
  modelview_matrix = sceneTranform * look_at_inv;

  // Make shader variable mappings 
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));

  glBindVertexArray (vao_frame);
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

    glBindVertexArray (vao_vec[i]);
    glDrawArrays(GL_TRIANGLES, 0, scenetriangles[i].size()/6);
  }
}

int main(int argc, char** argv)
{
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

