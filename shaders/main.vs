#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNorm;

out vec2 FragUV;
out vec3 FragPos;
out vec3 FragNorm;

uniform mat4 camView;
uniform mat4 model;

void main() {
   gl_Position = camView * vec4(vertexPos, 1.0f);
   FragPos = vec3(model * vec4(vertexPos, 1.0f));

   //Accounts for any non-uniform stretching in model
   FragNorm = mat3(transpose(inverse(model))) * vertexNorm;
   FragUV = vertexUV;
}

