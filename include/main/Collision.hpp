#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <entityx/entityx.h>
namespace ex = entityx;

#include "main/Components.hpp"


class CollisionSystem : public ex::System<CollisionSystem> {
   public:
      explicit CollisionSystem(ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      void wallCollision(ex::Entity&, Room&);

      Position* camPos;
};




#endif // COLLISION_HPP
