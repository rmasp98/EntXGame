////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Main.hpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef LEVEL_HPP
#define LEVEL_HPP

//Graphics library headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <entityx/entityx.h>
namespace ex = entityx;

//Game headers
#include "main/Renderer.hpp"
#include "main/ObjectGen.hpp"
#include "main/Movement.hpp"
#include "main/BuildRoom.hpp"
#include "main/Collision.hpp"


//Initialise graphics libaries
int initGraphics(GLFWwindow*&);

//The entity system class
class Level : public ex::EntityX {
   public:
      //add a new system here. I think the order matters
      explicit Level(GLFWwindow* window) {
         systems.add<ObjectSystem>(entities);
         systems.add<RoomSystem>(entities);
         systems.add<MoveSystem>(window);
         systems.add<CollisionSystem>();
         systems.add<RenderSystem>(entities);
         systems.configure();
      }

      void update(ex::TimeDelta dT) {
         systems.update_all(dT);
      }

};





#endif // LEVEL_HPP
