#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <sstream>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <entityx/entityx.h>
namespace ex = entityx;

#include "main/Components.hpp"


class RenderSystem : public ex::System<RenderSystem> {
   public:
      explicit RenderSystem(ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      void genBuffers(ex::Entity&, Renderable&, Shader&);
      void drawScene(Renderable&, Shader&);
      void addLight(ex::EntityManager&);
      Camera* cam;

};





#endif // RENDERER_HPP
