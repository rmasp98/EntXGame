#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <glm/glm.hpp>
#include <GL/gl.h>
#include <vector>


struct Position {
   Position(glm::vec3 posIn) : pos(posIn) {}

   glm::vec3 pos;
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
   GLuint progID, VAO, vertID, uvID, normID, indID, indSize, texID;
   glm::mat4 modelMat;
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

   glm::mat4 projection, view;
};



#endif // COMPONENTS_HPP
