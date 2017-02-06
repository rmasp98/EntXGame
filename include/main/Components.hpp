////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                               Components.hpp                                       //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

//Generic libraries
#include <vector>

//Graphics libraries
#include <glm/glm.hpp>
#include <GL/gl.h>

//////////////////////////////////////////////////////////////////////////////////////////
//List of global components. These components are used in all aspects of the game


//Just holds some values for the camera
struct Camera {
   Camera(glm::mat4 projIn, glm::mat4 viewIn) : projection(projIn), view(viewIn) {}
   Camera() {}

   glm::mat4 projection, view;
};



//This is a holder for the program ID
struct Shader {
   Shader(GLuint shaderIn) : progID(shaderIn) {}

   GLuint progID;
};



//An object with position. There is also a buffer for collision detection
struct Position {
   Position(glm::vec3 posIn, GLfloat buffIn) : pos(posIn), buffer(buffIn) {}

   glm::vec3 pos;
   GLfloat buffer;
};



//An object that can be rendered. Includes vertices, normals, uv coordinates, model matrix and various IDs:
//Vertex Array object, graphics ids for verts, uv coords, norms, VBO indices; number of verts and texture ID
struct Renderable {
   Renderable(std::vector<glm::vec3> vertIn, std::vector<glm::vec3> normIn, std::vector<glm::vec2> uvIn, GLint texIn)
   : verts(vertIn), norms(normIn), uvs(uvIn), texID(texIn) {}

   std::vector<glm::vec3> verts, norms;
   std::vector<glm::vec2> uvs;
   GLuint VAO, vertID, uvID, normID, indID, indSize, texID;
   glm::mat4 modelMat;
};





/////////////////////////////////////////////////////////////////////////////////////////////////
//This is a list of components required for the level



//An object that moves. This includes the maximum speed, the accerlation and current velocity
struct Acceleration {
   Acceleration(GLfloat maxVIn, GLfloat accIn)
                : maxSpeed(maxVIn), accel(accIn), vel(glm::vec3(0.0f)) {}

   GLfloat maxSpeed, accel;
   glm::vec3 vel;
};


//This is mostly for Cameras. This includes the pan speed, current angle, facing direction, travelling and right direction
struct Direction {
   Direction(GLfloat speedIn, glm::vec2 angIn, glm::vec3 faceIn)
          : mouseSpeed(speedIn), angle(angIn), facing(faceIn),
            dir(glm::vec3(faceIn.x, 0.0f, faceIn.z)), right(glm::vec3(faceIn.z, 0.0f, -faceIn.x)) {}

   GLfloat mouseSpeed;
   glm::vec2 angle;
   glm::vec3 facing, dir, right;
};



//This is for jumping objects. This includes xy movement while jumping, whether it is jumping currently
//speed that in starts in jump and accerlation due to gravity
struct Jump {
   Jump(GLfloat jumpVIn, GLfloat gIn) : jumpSpeed(jumpVIn), gravity(gIn) {}

   glm::vec2 jumpMove;
   bool isJump;
   GLfloat jumpSpeed, gravity;
};



//This is for light entities. Contains the ambient, diffuse and specular components of the light
struct Light {
   Light(glm::vec3 ambIn, glm::vec3 difIn, glm::vec3 specIn)
   : ambient(ambIn), diffuse(difIn), specular(specIn) {}

   glm::vec3 ambient, diffuse, specular;
};




//This is mostly for the room. Includes the blocks, boundary and normals for the walls, floor and roof
//This is used for collision detection
struct Room {
   Room() {}

   std::vector< glm::tvec2<GLint> > blocks;
   std::vector< std::vector<GLfloat> > bound;
   std::vector<glm::vec3> norms;
};



//This is a holder for anything that can move and hence collide
struct Collidable {
   Collidable() {}

};



//Holds the direction and whether the object is being pushed
struct Push {
   Push() : pushDir(glm::vec3(0.0f)), isPush(false) {}

   glm::vec3 pushDir;
   bool isPush;
};





///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Components required for the Menus







#endif // COMPONENTS_HPP
