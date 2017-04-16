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
#include "main/Movement.hpp"
#include "main/BuildRoom.hpp"
#include "main/Collision.hpp"
#include "main/Menu.hpp"

//Graphics library headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <entityx/entityx.h>
namespace ex = entityx;


//Initialise graphics libaries
int initGraphics(GLFWwindow*&);

//The entity system class
class Level : public ex::EntityX {
   public:
      //add a new system here. I think the order matters
      explicit Level(GLFWwindow* window);
      void update(ex::TimeDelta dT);

   protected:
      bool isMenu;

};




class Menu : public ex::EntityX {
   public:
      explicit Menu(GLFWwindow* window);
      void update(ex::TimeDelta dT);

};





#endif // LEVEL_HPP
