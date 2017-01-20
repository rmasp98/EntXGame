#ifndef BUILD_ROOM_HPP
#define BUILD_ROOM_HPP

#include <vector>
#include <time.h>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <entityx/entityx.h>
namespace ex = entityx;

#include "main/Components.hpp"


class RoomSystem : public ex::System<RoomSystem> {
   public:
      explicit RoomSystem(ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:

      std::vector< glm::tvec2<GLint> > createBlocks(GLint);

};

#endif // BUILD_ROOM_HPP
