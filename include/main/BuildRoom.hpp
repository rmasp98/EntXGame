#ifndef BUILD_ROOM_HPP
#define BUILD_ROOM_HPP

#include <vector>
#include <time.h>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <entityx/entityx.h>
namespace ex = entityx;

#include "main/Components.hpp"
#include "common/shader.hpp"
#include "common/texture.hpp"


struct roomComps {
   std::vector< glm::tvec2<GLint> > blocks;
   std::vector< std::vector<GLfloat> > bound;
   std::vector<glm::vec3> norms;
};


class RoomSystem : public ex::System<RoomSystem> {
   public:
      explicit RoomSystem(ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:

      std::vector< glm::tvec2<GLint> > createBlocks(GLint);
      void createBound(roomComps&);
      void buildRoom(ex::Entity&, roomComps&);

};

#endif // BUILD_ROOM_HPP
