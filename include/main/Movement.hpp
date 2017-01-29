////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Movement.hpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

//Generic libraries
#include <cmath>

//Graphic libraries
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entityx/entityx.h>
namespace ex = entityx;

//Game headers
#include "main/Components.hpp"


class MoveSystem : public ex::System<MoveSystem> {
   public:
      explicit MoveSystem(GLFWwindow*);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      GLFWwindow* win;
      GLint winXcen, winYcen;

      void moveObject(ex::Entity&, Position&, Acceleration&, Direction&, float);
      void changeDirection(Camera&, Direction&, GLfloat);
};




#endif // MOVEMENT_HPP
