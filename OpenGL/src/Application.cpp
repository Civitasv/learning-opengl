#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "IndexBuffer.h"
#include "Log.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

int main(void) {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) return -1;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
        -1.5f, -1.5f, 0.0f, 0.0f,  // 0
        1.5f,  -1.5f, 1.0f, 0.0f,  // 1
        1.5f,  1.5f,  1.0f, 1.0f,  // 2
        -1.5f, 1.5f,  0.0f, 1.0f   // 3
    };

    unsigned int indices[] = {0, 1, 2, 2, 3, 0};

    GLCall(glEnable(GL_BLEND));
    // R = r_src * sfactor + r_dest * dfactor
    // G = g_src * sfactor + g_dest * dfactor
    // B = b_src * sfactor + b_dest * dfactor
    // A = a_src * sfactor + a_dest * dfactor
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    Renderer renderer;

    // vertex array object
    VertexArray va;
    // vertex buffer object
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));
    // specify layout in vertex buffer
    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);

    // index buffer object, specify how I want to draw this layout.
    IndexBuffer ib(indices, 6);

    va.AddBuffer(vb, ib, layout);

    // 4 * 3
    glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

    Shader shader("res/shaders/Basic.shader");
    shader.Bind();
    shader.SetUniformMat4f("u_MVP", proj);

    // unbind everything
    shader.Unbind();
    va.Unbind();
    vb.Unbind();
    ib.Unbind();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
      /* Render here */
      renderer.Clear();

      Texture texture("res/textures/icon.png");
      texture.Bind();

      shader.Bind();
      shader.SetUniform1i("u_Texture", 0);

      renderer.Draw(va, shader, ib.GetCount());

      /* Swap front and back buffers */
      glfwSwapBuffers(window);
      glfwSwapInterval(1);

      /* Poll for and process events */
      GLCall(glfwPollEvents());
    }
  }
  glfwTerminate();
  return 0;
}