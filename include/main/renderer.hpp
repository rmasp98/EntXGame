#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <entityx/entityx.h>
namespace ex = entityx;

#include "main/components.hpp"


class RenderSystem : public ex::System<RenderSystem> {
   public:
      explicit RenderSystem(ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      void genBuffers(ex::Entity&, Renderable&);
      void drawScene(Renderable&);
      Camera cam;

};





#endif // RENDERER_HPP
