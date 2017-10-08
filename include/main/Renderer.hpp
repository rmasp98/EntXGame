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
      explicit RenderSystem(ex::EntityManager&);
      void configure(ex::EventManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;
      void receive(const GenBuffers&);
      void receive(const PrepShadowMap&);

   protected:
      GLFWwindow* window;
      GLuint depthMapFBO, gBuffer, depthCubemap, lightingPID, menuPID, deferPID, shadowPID;
      GLuint gPosition, gNormal, gAlbedoSpec, quadVAO;
      GLuint winHeight, winWidth, shadowHeight, shadowWidth;
      GLfloat gamma, farPlane, nearPlane;

      void prepGBuffer();
      void renderQuad();
      void genShadowMap(ex::EntityManager&);
      void genGBuffer(ex::EntityManager&);
      void deferredRender(ex::EntityManager&);
      void genBuffers(ex::Entity&, std::vector<glm::vec3> vertIn, std::vector<glm::vec3> normIn, std::vector<glm::vec2> uvIn);
      void drawScene(Renderable&, Material&, GLuint, ex::EntityManager& entM, ex::Entity&);
      void addLight(ex::EntityManager&, GLuint);
};


#endif // RENDERER_HPP
