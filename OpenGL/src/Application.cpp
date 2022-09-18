#include <iostream>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

static unsigned int CompileShader(unsigned int type, const std::string& source) {
  unsigned int id = glCreateShader(type);
  const char* src = source.c_str();
  // Set the source code in shader(id) to the source code in the array of strings specified by string(source). 
  glShaderSource(id, 1, &src, nullptr);
  // Compile the source code strings that have been stored in the shader object specified by shader.
  glCompileShader(id);

  // GET ERROR MESSAGE
  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char* message = (char*)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    std::cout << "FAILED to compile" << source << " shader!" << message << '\n';

    glDeleteShader(id);
    return 0;
  }
  return id;
}
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
  unsigned int program = glCreateProgram();
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);
  glValidateProgram(program);

  // vs and fs kinda like C++'s object file, so after linking, we can delete them.
  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}

int main(void) {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  // init glew
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    /* Problem: glewInit failed, something is seriously wrong. */
    std::cerr << "Error: " << glewGetErrorString(err) << '\n';
  }
  std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << '\n';

  float positions[6] = { -0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -0.5f };

  // STATE MACHINE
  unsigned int buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  // index: index of this attribute
  // size: the number of components of this attribute
  // stride: byte offset between two attributes
  // pointer: first location of this attribute
  // 0 表示该属性的 index，2 表示该属性有两个数据组成，GL_FLOAT 表示每个数据是 float
  // GL_FALSE 表示不要normalize，2*sizeof(float) 表示该属性第一个值和第二个值之间的间隔，
  // 0 表示该属性第一个值在数据（positions）中的位置
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

  std::string vertexShader =
    "#version 330 core\n"
    "\n"
    "layout(location=0) in vec4 position;" // specify paramters
    "\n"
    "void main() \n"
    "{\n"
    " gl_Position = position;\n"
    "}\n";
  std::string fragmentShader =
    "#version 330 core\n"
    "\n"
    "out vec4 color;" // specify return value
    "\n"
    "void main() \n"
    "{\n"
    " color=vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

  unsigned int shader = CreateShader(vertexShader, fragmentShader);
  glUseProgram(shader);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }
  glDeleteShader(shader);
  glEnableVertexAttribArray(0);

  glfwTerminate();
  return 0;
}