#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <GLFW/glfw3.h>
#include <entityx/entityx.h>
namespace ex = entityx;

#include "main/Renderer.hpp"
#include "main/ObjectGen.hpp"
#include "main/Movement.hpp"
#include "main/BuildRoom.hpp"
#include "main/Collision.hpp"


class Level : public ex::EntityX {
   public:
      explicit Level(GLFWwindow* window) {
         systems.add<ObjectSystem>(entities);
         systems.add<RoomSystem>(entities);
         systems.add<MoveSystem>(window);
         systems.add<CollisionSystem>(entities);
         systems.add<RenderSystem>(entities);
         systems.configure();
      }

      void update(ex::TimeDelta dT) {
         systems.update_all(dT);
      }

};





#endif // LEVEL_HPP
