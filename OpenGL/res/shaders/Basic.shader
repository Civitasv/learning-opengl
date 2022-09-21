#shader vertex
#version 330 core

layout(location = 0) in vec4 position;  // specify paramters

void main() { gl_Position = position; };

#shader fragment
#version 330 core

out vec4 color;  // specify return value

uniform vec4 u_Color;

void main() { color = u_Color; };