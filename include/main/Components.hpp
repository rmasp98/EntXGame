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
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/gl.h>

//Text libraries
#include <ft2build.h>
#include FT_FREETYPE_H

// Entityx libraries
#include <entityx/entityx.h>
namespace ex = entityx;


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
   : verts(vertIn), norms(normIn), uvs(uvIn), texID(texIn), modelMat(glm::mat4(1.0f)) {}

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

// (NOT A COMPONENT) struct for a single character in a font
struct character {
   glm::vec2 size, bearing, offset;
   glm::ivec2 advance;
};

// (NOT A COMPONENT) Struct of an atlas map of a font. Functions are in MenuGen.cpp
struct Atlas {
   GLuint texID, w, h;  // texture object, width and height of texture in pixels
   character c[128];

   Atlas(std::string, GLuint, FT_Library& ft);
   ~Atlas() { glDeleteTextures(1, &texID); };
};

//Holds the font and font colours. Lo = standard, Hi = selected
struct Font {
   Font(glm::vec3 loColIn, glm::vec3 hiColIn, Atlas* fontIn) :
        colour(loColIn), loColour(loColIn), hiColour(hiColIn), atlas(fontIn) {};
   ~Font() { delete atlas; };

   glm::vec3 colour, loColour, hiColour;
   Atlas* atlas;
};


struct Clickable {
   Clickable(GLfloat xMin, GLfloat yMin, GLfloat xMax, GLfloat yMax, GLuint button) :
             bound(glm::vec4(xMin, yMin, xMax, yMax)), buttonID(button) {};

   glm::vec4 bound;
   GLuint buttonID;
};

//Possible components:
//  - MenuID (identifies which menu a button is associated with)
//  - LinkMenu (contains menuID of linked menu)
//  - NonLinkedButtons (this will somehow indicate a function that the button will run)






#endif // COMPONENTS_HPP
