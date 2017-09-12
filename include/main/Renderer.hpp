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
#include "common/shader.hpp"

//Generic libraries
#include <sstream>

//Graphic libraries
#include <GLFW/glfw3.h>


class RenderSystem : public ex::System<RenderSystem>, public ex::Receiver<RenderSystem> {
   public:
      explicit RenderSystem(ex::EntityManager&, GLFWwindow*);
      void configure(ex::EventManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;
      void receive(const GenBuffers&);

   protected:
      GLFWwindow* window;
      GLuint depthMapFBO, depthCubemap, mainPID, menuPID, shadowPID;

      void prepShadowMap();
      void genBuffers(ex::Entity&, GLuint);
      void drawScene(Renderable&, GLuint, ex::EntityManager& entM, ex::Entity&);
      void addLight(ex::EntityManager&, GLuint);
};


#endif // RENDERER_HPP
