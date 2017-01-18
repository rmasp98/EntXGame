#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <entityx/entityx.h>
namespace ex = entityx;

#include "main/components.hpp"


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
