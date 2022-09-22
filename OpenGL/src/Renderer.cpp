#include "Renderer.h"

#include <iostream>

#include "GL/glew.h"
#include "Log.h"

void Renderer::Clear() const { GLCall(glClear(GL_COLOR_BUFFER_BIT)); }

void Renderer::Draw(const VertexArray& va, const Shader& shader,
                    int count) const {
  shader.Bind();

  // I can just bind VAO, it will bind VBO and vertex layout and IBO for us.
  va.Bind();

  GLCall(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
}