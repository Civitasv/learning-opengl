#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "IndexBuffer.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

struct ShaderProgramSource {
  std::string VertexSource;
  std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) {
  std::ifstream stream(filepath);
  enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };
  std::string line;
  std::stringstream ss[2];
  ShaderType type = ShaderType::NONE;

  while (getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos) {
        // vertex
        type = ShaderType::VERTEX;
      } else if (line.find("fragment") != std::string::npos) {
        // fragment
        type = ShaderType::FRAGMENT;
      }
    } else {
      ss[(int)type] << line << '\n';
    }
  }
  return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type,
                                  const std::string& source) {
  GLCall(unsigned int id = glCreateShader(type));
  const char* src = source.c_str();
  // Set the source code in shader(id) to the source code in the array of
  // strings specified by string(source).
  GLCall(glShaderSource(id, 1, &src, nullptr));
  // Compile the source code strings that have been stored in the shader object
  // specified by shader.
  GLCall(glCompileShader(id));

  // GET ERROR MESSAGE
  int result;
  GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
  if (result == GL_FALSE) {
    int length;
    GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
    char* message = (char*)alloca(length * sizeof(char));
    GLCall(glGetShaderInfoLog(id, length, &length, message));
    std::cout << "FAILED to compile" << source << " shader!" << message << '\n';

    GLCall(glDeleteShader(id));
    return 0;
  }
  return id;
}

static unsigned int CreateShader(const std::string& vertexShader,
                                 const std::string& fragmentShader) {
  GLCall(unsigned int program = glCreateProgram());
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  GLCall(glAttachShader(program, vs));
  GLCall(glAttachShader(program, fs));

  GLCall(glLinkProgram(program));
  GLCall(glValidateProgram(program));

  // vs and fs kinda like C++'s object file, so after linking, we can delete
  // them.
  GLCall(glDeleteShader(vs));
  GLCall(glDeleteShader(fs));

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
  {
    float positions[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
    };

    unsigned int indices[] = {0, 1, 2, 2, 3, 0};

    // vertex array object
    VertexArray va;
    // vertex buffer object
    VertexBuffer vb(positions, 4 * 2 * sizeof(float));
    // specify layout in vertex buffer
    VertexBufferLayout layout;
    layout.Push<float>(2); 

    // index buffer object, specify how I want to draw this layout.
    IndexBuffer ib(indices, 6);

    va.AddBuffer(vb, ib, layout);

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

    unsigned int program =
        CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(program));

    GLCall(int location = glGetUniformLocation(program, "u_Color"));
    ASSERT(location != -1);

    // unbind everything
    GLCall(glUseProgram(0));
    va.Unbind();
    vb.Unbind();
    ib.Unbind();

    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
      /* Render here */
      GLCall(glClear(GL_COLOR_BUFFER_BIT));

      GLCall(glUseProgram(program));
      GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

      // I can just bind VAO, it will bind VBO and vertex layout and IBO for us.
      va.Bind();

      GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

      if (r > 1.0f)
        increment = -0.05f;
      else if (r < 0.0f)
        increment = 0.05f;
      r += increment;

      /* Swap front and back buffers */
      glfwSwapBuffers(window);
      glfwSwapInterval(1);

      /* Poll for and process events */
      GLCall(glfwPollEvents());
    }
    GLCall(glDeleteProgram(program));
    GLCall(glEnableVertexAttribArray(0));
  }
  glfwTerminate();
  return 0;
}