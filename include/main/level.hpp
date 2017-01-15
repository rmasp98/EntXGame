#ifndef LEVEL_HPP
#define LEVEL_HPP


#include <entityx/entityx.h>
namespace ex = entityx;

#include "main/renderer.hpp"
#include "main/objectGen.hpp"


class Level : public ex::EntityX {
   public:
      explicit Level() {
         systems.add<objGenSystem>(entities);
         systems.add<RenderSystem>(entities);
         systems.configure();
      }

      void update(ex::TimeDelta dT) {
         systems.update_all(dT);
      }

};





#endif // LEVEL_HPP
