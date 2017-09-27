////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Renderer.cpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Renderer.hpp"




RenderSystem::RenderSystem(ex::EntityManager &entM, GLFWwindow* winIn) {

   // Create the Frame buffer to store the shadowmap
   glGenFramebuffers(1, &depthMapFBO);

   window = winIn;
   prepShadowMap();

   quadVAO = 0;

   //Load the shaders (need to add shader names to config file)
   mainPID = LoadShaders("shaders/main.vs", "shaders/main.fs");
   menuPID = LoadShaders("shaders/menu.vs", "shaders/menu.fs");
   deferPID = LoadShaders("shaders/defer.vs", "shaders/defer.fs");
   shadowPID = LoadShaders("shaders/shadow.vs", "shaders/shadow.gs", "shaders/shadow.fs");






   glGenFramebuffers(1, &gBuffer);
   glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

   // position color buffer
   glGenTextures(1, &gPosition);
   glBindTexture(GL_TEXTURE_2D, gPosition);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 960, 540, 0, GL_RGB, GL_FLOAT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
   // normal color buffer
   glGenTextures(1, &gNormal);
   glBindTexture(GL_TEXTURE_2D, gNormal);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 960, 540, 0, GL_RGB, GL_FLOAT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
   // color + specular color buffer
   glGenTextures(1, &gAlbedoSpec);
   glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 960, 540, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
   // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
   GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
   glDrawBuffers(3, attachments);
   // create and attach depth buffer (renderbuffer)
   GLuint rboDepth;
   glGenRenderbuffers(1, &rboDepth);
   glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 960, 540);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

   // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
   //      std::cout << "Framebuffer not complete!" << std::endl;

   glBindFramebuffer(GL_FRAMEBUFFER, 0);


   // GLint texture_units;
   // glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
   // std::cout << "hello " << texture_units << std::endl;
}


void RenderSystem::configure(ex::EventManager& evnM) {
   evnM.subscribe<GenBuffers>(*this);
   evnM.subscribe<PrepShadowMap>(*this);
}




void RenderSystem::update(ex::EntityManager &entM, ex::EventManager &evnM, ex::TimeDelta dT) {

   //Creates the model matrix based on the objects current position
   entM.each<Position, Renderable>([this](ex::Entity entity, Position& pos, Renderable& mat) {
      mat.modelMat = glm::translate(glm::mat4(1.0f), pos.pos);
   });

   GLuint currScrn;
   entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
      currScrn = screen.id;
   });

   if (currScrn >= 10) {
      // Hides the cursor
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

      entM.each<Light, Position, Shadow>([this, &entM](ex::Entity eNull, Light& lNull, Position& light, Shadow& shadow) {
         genShadowMap(entM, light.pos, shadow.depthMap);
      });
      glCullFace(GL_BACK);
      glViewport(0, 0, 960, 540);





      glDisable(GL_BLEND);
      glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glUseProgram(deferPID);

      glUniform1i(glGetUniformLocation(deferPID, "material.diffuse"), 0);

      entM.each<Camera, Position>([this](ex::Entity ent, Camera& cam, Position& pos) {
         //Send camera information to the buffer
         glm::mat4 camView = cam.projection * cam.view;
         glUniformMatrix4fv(glGetUniformLocation(deferPID, "camView"), 1, GL_FALSE, &camView[0][0]);
         glUniform3fv(glGetUniformLocation(deferPID, "viewPos"), 1, &(pos.pos[0]));
      });

      //Passes camera and model matrix, and then renders each object
      entM.each<Renderable, Level>([this, &entM](ex::Entity entity, Renderable &mesh, Level& null) {
         // Pass material shininess to shader
         glUniform1f(glGetUniformLocation(deferPID, "material.shininess"), 20.0);

         drawScene(mesh, deferPID, entM, entity);
      });





      glEnable(GL_BLEND);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glUseProgram(mainPID);

      glUniform1i(glGetUniformLocation(mainPID, "gPosition"), 0);
      glUniform1i(glGetUniformLocation(mainPID, "gNormal"), 1);
      glUniform1i(glGetUniformLocation(mainPID, "gAlbedoSpec"), 2);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, gPosition);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, gNormal);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);



      //Pass the lights to graphics card
      addLight(entM, mainPID);

      entM.each<Camera, Position>([this](ex::Entity ent, Camera& cam, Position& pos) {
         glUniform3fv(glGetUniformLocation(mainPID, "viewPos"), 1, &(pos.pos[0]));
      });

      // Passes gamma value to graphics
      glUniform1f(glGetUniformLocation(mainPID, "gamma"), 2.2);
      GLfloat farPlane = 25.0f;
      glUniform1f(glGetUniformLocation(mainPID, "farPlane"), farPlane);

      //This is a bodge, figure out how to fix!
      GLuint iLight = 0;
      entM.each<Light, Shadow>([this, &iLight](ex::Entity entity, Light& null, Shadow& shadow) {
         std::stringstream dmName;
         dmName << "light2[" << iLight++ << "].depthMap";

         glUniform1i(glGetUniformLocation(mainPID, &(dmName.str())[0]), 3 + iLight);

         glActiveTexture(GL_TEXTURE3 + iLight);
         glBindTexture(GL_TEXTURE_CUBE_MAP, shadow.depthMap);
      });

      renderQuad();

      // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
      // ----------------------------------------------------------------------------------
      glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
      // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
      // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
      // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
      glBlitFramebuffer(0, 0, 960, 540, 0, 0, 960, 540, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

   }



   //Passes camera and model matrix, and then renders each object
   entM.each<Renderable, MenuID>([this, &entM, &currScrn](ex::Entity entity, Renderable& mesh, MenuID& menu) {
      if (currScrn == menu.id) {
         glUseProgram(menuPID);

         //Passes the type of render for the menu shader (could just split shaders)
         ex::ComponentHandle<Font> font = entity.component<Font>();
         if (font)
            glUniform1i(glGetUniformLocation(menuPID, "renderType"), 0);
         else if (mesh.texID != 0)
            glUniform1i(glGetUniformLocation(menuPID, "renderType"), 1);
         else
            glUniform1i(glGetUniformLocation(menuPID, "renderType"), 2);

         // Reenables cursor for the menu
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
         drawScene(mesh, menuPID, entM, entity);
      }
   });


   //Reset the VAO
   glBindVertexArray(0);
}







void RenderSystem::receive(const GenBuffers& gen) {

   // Need to figure way to remove unused buffers (maybe, does use same ids...)
   genBuffers(gen.entity, gen.verts, gen.norms, gen.uvs);

}






void RenderSystem::receive(const PrepShadowMap& sMap) {

   const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
   glBindTexture(GL_TEXTURE_CUBE_MAP, sMap.shadow->depthMap);
   for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}





void RenderSystem::renderQuad() {
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        GLuint quadVBO;
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}






void RenderSystem::prepShadowMap() {


}






void RenderSystem::genShadowMap(ex::EntityManager& entM, glm::vec3 lightPos, GLuint depthMap) {

   const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
   GLfloat nearPlane = 1.0f, farPlane = 25.0f;
   glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, nearPlane, farPlane);
   std::vector<glm::mat4> shadowTransforms;
   shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
   shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
   shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
   shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
   shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
   shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

   glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
   glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

   glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);

   glClear(GL_DEPTH_BUFFER_BIT);
   glUseProgram(shadowPID);
   for (GLuint iCube = 0; iCube < 6; ++iCube) {
      std::stringstream matName;
      matName << "shadowMatrices[" << iCube << "]";

      glm::mat4 tempMat = shadowTransforms[iCube];
      glUniformMatrix4fv(glGetUniformLocation(shadowPID, &(matName.str())[0]), 1, GL_FALSE, &tempMat[0][0]);
   }

   glUniform1f(glGetUniformLocation(shadowPID, "farPlane"), farPlane);
   glUniform3fv(glGetUniformLocation(shadowPID, "lightPos"), 1, &(lightPos[0]));

   //Passes camera and model matrix, and then renders each object
   entM.each<Renderable, Level>([this, &entM](ex::Entity entity, Renderable &mesh, Level& null) {
      drawScene(mesh, shadowPID, entM, entity);
   });
}








//This creates the VAOs, which are later used to quick switch between objects to render
void RenderSystem::genBuffers(ex::Entity& ent, std::vector<glm::vec3> verts, std::vector<glm::vec3> norms, std::vector<glm::vec2> uvs) {

   ex::ComponentHandle<Renderable> eVecs = ent.component<Renderable>();
   if (eVecs) {
      std::vector<unsigned short> inds;
      std::vector<glm::vec2> uvsInds;
      std::vector<glm::vec3> vertInds, normInds;

      //This performs VBO indexing
      indexVBO(verts, uvs, norms, inds, vertInds, uvsInds, normInds);
      eVecs->indSize = inds.size();

      //Generate and bind VAO
      glGenVertexArrays(1, &eVecs->VAO);
      glBindVertexArray(eVecs->VAO);

      //Bind vertices to shader
      GLuint vertID;
      glGenBuffers(1, &vertID);
      glBindBuffer(GL_ARRAY_BUFFER, vertID);
      glBufferData(GL_ARRAY_BUFFER, vertInds.size() * sizeof(glm::vec3), &vertInds[0], GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      //Bind texture locations to shader
      GLuint uvID;
      glGenBuffers(1, &uvID);
      glBindBuffer(GL_ARRAY_BUFFER, uvID);
      glBufferData(GL_ARRAY_BUFFER, uvsInds.size() * sizeof(glm::vec2), &uvsInds[0], GL_STATIC_DRAW);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

      //Bind normals to shader
      GLuint normID;
      glGenBuffers(1, &normID);
      glBindBuffer(GL_ARRAY_BUFFER, normID);
      glBufferData(GL_ARRAY_BUFFER, normInds.size() * sizeof(glm::vec3), &normInds[0], GL_STATIC_DRAW);
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      //Bind VBO indices to shader
      GLuint indID;
      glGenBuffers(1, &indID);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indID);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(unsigned short), &inds[0] , GL_STATIC_DRAW);

      //Unbind VAO
      glBindVertexArray(0);
   }
}










void RenderSystem::drawScene(Renderable& mesh, GLuint prog, ex::EntityManager& entM, ex::Entity& entity) {

   //Rebind the objects VAO
   glBindVertexArray(mesh.VAO);

   //Send object's model matrix
   glUniformMatrix4fv(glGetUniformLocation(prog, "model"), 1, GL_FALSE, &mesh.modelMat[0][0]);
   // Passes colour of font to graphic card
   glUniform3fv(glGetUniformLocation(prog, "colour"), 1, &(mesh.colour[0]));

   //Resets the texture and binds correct texture
   if (mesh.texID != 0) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mesh.texID);
   }

   //Render the object
   glDrawElements(GL_TRIANGLES, mesh.indSize, GL_UNSIGNED_SHORT, (void*)0);

   //Unbind the VAO for the next object
   glBindVertexArray(0);

}





void RenderSystem::addLight(ex::EntityManager& entM, GLuint pID) {

   //Runs over all light entities to send the information to the buffer
   GLuint iNum = 0;
   entM.each<Light, Position>([this, &iNum, &pID]
                                     (ex::Entity entity, Light& l, Position& p) {

      //glUseProgram(pID);

      //Creates a custom name to pass the lights into an array
      std::stringstream bName;
      bName << "light[" << iNum++ << "].";

      glUniform3f(glGetUniformLocation(pID, &(bName.str() + "pos")[0]),
                                       p.pos.x, p.pos.y, p.pos.z);

      glUniform3f(glGetUniformLocation(pID, &(bName.str() + "ambient")[0]),
                                       l.ambient.x, l.ambient.y, l.ambient.z);

      glUniform3f(glGetUniformLocation(pID, &(bName.str() + "diffuse")[0]),
                                       l.diffuse.x, l.diffuse.y, l.diffuse.z);

      glUniform3f(glGetUniformLocation(pID, &(bName.str() + "specular")[0]),
                                       l.specular.x, l.specular.y, l.specular.z);

      glUniform1f(glGetUniformLocation(pID, &(bName.str() + "linear")[0]), l.linear);
      glUniform1f(glGetUniformLocation(pID, &(bName.str() + "quad")[0]), l.quad);
   });

   //Passes the number of lights to the buffer
   glUniform1i(glGetUniformLocation(pID, "lightNum"), iNum);

}
