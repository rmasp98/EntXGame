#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <glm/glm.hpp>
#include <GL/gl.h>
#include <vector>


struct Position {
   Position(glm::vec3 posIn) : pos(posIn) {}

   glm::vec3 pos;
};



struct Acceleration {
   Acceleration(GLfloat maxVIn, GLfloat accIn, glm::vec3 dirIn)
                : maxSpeed(maxVIn), accel(accIn), speed(glm::vec3(0.0f)), dir(dirIn), right(glm::vec3(dirIn.z, 0.0f, -dirIn.x)) {}

   GLfloat maxSpeed, accel;
   glm::vec3 speed, dir, right;
};



struct Jump {
   Jump(GLfloat jumpVIn, GLfloat gIn) : jumpSpeed(jumpVIn), gravity(gIn) {}

   glm::vec2 jumpMove;
   bool isJump, isPreJump;
   GLfloat jumpSpeed, gravity;
};




struct Light {
   Light(glm::vec3 ambIn, glm::vec3 difIn, glm::vec3 specIn)
   : ambient(ambIn), diffuse(difIn), specular(specIn) {}

   glm::vec3 ambient, diffuse, specular;
};


struct Renderable {
   Renderable(std::vector<glm::vec3> vertIn, std::vector<glm::vec3> normIn, std::vector<glm::vec2> uvIn)
   : verts(vertIn), norms(normIn), uvs(uvIn) {}

   std::vector<glm::vec3> verts, norms;
   std::vector<glm::vec2> uvs;
   GLuint VAO, vertID, uvID, normID, indID, indSize, texID;
   glm::mat4 modelMat;
};



struct Shader {
   Shader(GLuint shaderIn) : progID(shaderIn) {}

   GLuint progID;
};


struct Texture {
   Texture(GLuint idIn) : textID(idIn) {}

   GLuint textID;
};



struct Collidable {
   Collidable() {}

};


struct Camera {
   Camera(glm::mat4 projIn, glm::mat4 viewIn) : projection(projIn), view(viewIn) {}
   Camera() {}

   glm::mat4 projection, view;
};



#endif // COMPONENTS_HPP
