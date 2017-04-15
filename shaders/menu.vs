#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
out vec2 FragUV;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(vertexPos, 1.0);
    FragUV = vertexUV;
}
