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
   Acceleration(GLfloat maxVIn, GLfloat accIn)
                : maxSpeed(maxVIn), accel(accIn), vel(glm::vec3(0.0f)) {}

   GLfloat maxSpeed, accel;
   glm::vec3 vel;
};



struct Direction {
   Direction(GLfloat speedIn, glm::vec2 angIn, glm::vec3 faceIn)
          : mouseSpeed(speedIn), angle(angIn), facing(faceIn),
            dir(glm::vec3(faceIn.x, 0.0f, faceIn.z)), right(glm::vec3(faceIn.z, 0.0f, -faceIn.x)) {}

   GLfloat mouseSpeed;
   glm::vec2 angle;
   glm::vec3 facing, dir, right;
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
   Renderable(std::vector<glm::vec3> vertIn, std::vector<glm::vec3> normIn, std::vector<glm::vec2> uvIn, GLint texIn)
   : verts(vertIn), norms(normIn), uvs(uvIn), texID(texIn) {}

   std::vector<glm::vec3> verts, norms;
   std::vector<glm::vec2> uvs;
   GLuint VAO, vertID, uvID, normID, indID, indSize, texID;
   glm::mat4 modelMat;
};




struct Room {
   Room(std::vector< glm::tvec2<GLint> > blkIn) : blocks(blkIn) {}

   std::vector< glm::tvec2<GLint> > blocks;
};



struct Shader {
   Shader(GLuint shaderIn) : progID(shaderIn) {}

   GLuint progID;
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
