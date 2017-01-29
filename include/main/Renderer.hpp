////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Renderer.hpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef RENDERER_HPP
#define RENDERER_HPP

//Generic libraries
#include <sstream>

//Graphics libraries
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <entityx/entityx.h>
namespace ex = entityx;

//Game headers
#include "main/Components.hpp"
#include "common/vboIndexer.hpp"


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
