////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Movement.hpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

//Game headers
#include "main/Components.hpp"

//Generic libraries
#include <cmath>

//Graphic libraries
#include <GLFW/glfw3.h>


class MoveSystem : public ex::System<MoveSystem>, public ex::Receiver<MoveSystem> {
   public:
      explicit MoveSystem(GLFWwindow*);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      GLFWwindow* win;
      GLint winXcen, winYcen, delay;
      bool isBEV, isUp, isDown, isRelease;
      glm::vec3 bevPos, currView, viewOrient, oldBevPos, viewPos, oldViewPos;

      void moveObject(ex::Entity&, Position&, Acceleration&, Direction*, GLfloat);
      void moveObject(ex::Entity&, Position&, Acceleration&, GLfloat);
      void changeDirection(Camera&, Direction&, GLfloat);
      void moveBEV(Position&);
};




#endif // MOVEMENT_HPP
