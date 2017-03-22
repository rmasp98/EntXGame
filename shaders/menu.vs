#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
//layout(location = 2) in vec3 vertexNorm;

out vec2 FragUV;

void main()
{
    gl_Position = vec4(vertexPos.x, vertexPos.y, vertexPos.z, 1.0);
    FragUV = vertexUV;
}
