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


class MoveSystem : public ex::System<MoveSystem>, public ex::Receiver<MoveSystem> {
   public:
      explicit MoveSystem(GLFWwindow*);
      void configure(ex::EventManager& evnM) override;
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;
      void receive(const PushEvent&);

   protected:
      GLFWwindow* win;
      GLint winXcen, winYcen;

      void moveObject(ex::Entity&, Position&, Acceleration&, Direction*, GLfloat);
      void moveObject(Position&, Acceleration&, GLfloat);
      void changeDirection(Camera&, Direction&, GLfloat);
};




#endif // MOVEMENT_HPP
