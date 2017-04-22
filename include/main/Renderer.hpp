////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Renderer.hpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef RENDERER_HPP
#define RENDERER_HPP

//Game headers
#include "main/Components.hpp"
#include "common/vboIndexer.hpp"

//Generic libraries
#include <sstream>

//Graphic libraries
#include <GLFW/glfw3.h>


class RenderSystem : public ex::System<RenderSystem> {
   public:
      explicit RenderSystem(ex::EntityManager&, GLFWwindow*);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      GLFWwindow* window;

      void genBuffers(ex::Entity&, Renderable&, Shader&);
      void drawScene(Renderable&, Shader&, ex::EntityManager& entM);
      void addLight(ex::EntityManager&);
};


#endif // RENDERER_HPP
