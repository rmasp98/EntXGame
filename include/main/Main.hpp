////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Main.hpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef LEVEL_HPP
#define LEVEL_HPP

//Game headers
#include "main/Renderer.hpp"
#include "main/ObjectGen.hpp"
#include "main/Input.hpp"
#include "main/Movement.hpp"
#include "main/BirdsEyeView.hpp"
#include "main/BuildRoom.hpp"
#include "main/Collision.hpp"
#include "main/MenuGen.hpp"
#include "main/MenuControl.hpp"

//Graphics library headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <entityx/entityx.h>
namespace ex = entityx;

#include <fstream>


//Initialise graphics libaries
int initGraphics(GLFWwindow*&);

//The entity system class
class Game : public ex::EntityX {
   public:
      explicit Game(GLFWwindow*);
      void update(ex::TimeDelta, std::ofstream&);

      GLuint frameCount;

   protected:
      bool isMenu;


};


#endif // LEVEL_HPP
