#pragma once

#include "Shader.h"
#include "VertexArray.h"

class Renderer {
 private:
 public:
  void Clear() const;
  void Draw(const VertexArray& va, const Shader& shader, int count) const;
};