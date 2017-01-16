#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <GLFW/glfw3.h>
#include <entityx/entityx.h>
namespace ex = entityx;

#include "main/renderer.hpp"
#include "main/objectGen.hpp"
#include "main/movement.hpp"


class Level : public ex::EntityX {
   public:
      explicit Level(GLFWwindow* window) {
         systems.add<objGenSystem>(entities);
         systems.add<MoveSystem>(window);
         systems.add<RenderSystem>(entities);
         systems.configure();
      }

      void update(ex::TimeDelta dT) {
         systems.update_all(dT);
      }

};





#endif // LEVEL_HPP
