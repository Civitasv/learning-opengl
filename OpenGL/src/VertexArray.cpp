#include "VertexArray.h"

#include "GL/glew.h"
#include "IndexBuffer.h"
#include "Log.h"

VertexArray::VertexArray() { GLCall(glGenVertexArrays(1, &m_RendererID)); }

VertexArray::~VertexArray() { GLCall(glDeleteVertexArrays(1, &m_RendererID)); }

void VertexArray::AddBuffer(const VertexBuffer& vb, const IndexBuffer& ib,
                            const VertexBufferLayout& layout) {
  Bind();
  vb.Bind();
  ib.Bind();

  const auto& elements = layout.GetElements();
  unsigned int offset = 0;
  for (unsigned int i = 0; i < elements.size(); i++) {
    GLCall(glEnableVertexAttribArray(i));
    // index: index of this attribute
    // size: the number of components of this attribute
    // stride: byte offset between two attributes
    // pointer: first location of this attribute
    // 0 ��ʾ�����Ե� index��2 ��ʾ������������������ɣ�GL_FLOAT ��ʾÿ��������
    // float GL_FALSE ��ʾ��Ҫnormalize��2*sizeof(float)
    // ��ʾ�����Ե�һ��ֵ�͵ڶ���ֵ֮��ļ���� 0
    // ��ʾ�����Ե�һ��ֵ�����ݣ�positions���е�λ��
    // �������Ҳʹ�� VBO �� VAO ��
    GLCall(glVertexAttribPointer(i, elements[i].count, elements[i].type,
                                 elements[i].normalized, layout.GetStride(),
                                 (const void*)offset));
    offset += elements[i].count *
              VertexBufferElement::GetSizeOfType(elements[i].type);
  }
}

void VertexArray::Bind() const { GLCall(glBindVertexArray(m_RendererID)); }

void VertexArray::Unbind() const { GLCall(glBindVertexArray(0)); }
