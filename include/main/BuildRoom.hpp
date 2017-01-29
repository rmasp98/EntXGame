////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                BuildRoom.hpp                                       //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef BUILD_ROOM_HPP
#define BUILD_ROOM_HPP

//Generic headers
#include <vector>
#include <time.h>

//Graphics libraries
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <entityx/entityx.h>
namespace ex = entityx;

//Game headers
#include "main/Components.hpp"
#include "common/shader.hpp"
#include "common/texture.hpp"


//System for building the puzzle room
class RoomSystem : public ex::System<RoomSystem> {
   public:
      explicit RoomSystem(ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      std::vector< glm::tvec2<GLint> > createBlocks(GLint);
      void createBound(ex::ComponentHandle<Room>&);
      void buildRoom(ex::EntityManager&, ex::Entity&, ex::ComponentHandle<Room>&, GLuint);

};

#endif // BUILD_ROOM_HPP
