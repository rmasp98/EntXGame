////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Renderer.cpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Renderer.hpp"




RenderSystem::RenderSystem(ex::EntityManager &entM) {

   // Create the Frame buffer to store the shadowmap
   glGenFramebuffers(1, &depthMapFBO);

   Input* input;
   entM.each<Input>([this, &input](ex::Entity null, Input& tempInput) {
      input = &tempInput;
   });
   // Variables to eventually pull from a config file
   winWidth = input->winCen[0] * 2.0; winHeight = input->winCen[1] * 2.0;
   shadowHeight = 1024; shadowWidth = 1024;
   gamma = 2.2f; nearPlane = 1.0f; farPlane = 25.0f;
   quadVAO = 0; samples = 8; shadows = false;

   // Loaded the various shaders
   std::vector<std::string> shadow(3, "");
   shadow[0] = "shaders/shadow.vs";
   shadow[1] = "shaders/shadow.gs";
   shadow[2] = "shaders/shadow.fs";
   shadowPID = LoadShaders(shadow);

   std::vector<std::string> defer(2, "");
   defer[0] = "shaders/defer.vs";
   defer[1] = "shaders/defer.fs";
   deferPID = LoadShaders(defer);

   std::vector<std::string> lighting(2, "");
   lighting[0] = "shaders/lighting.vs";
   lighting[1] = "shaders/lighting.fs";
   lightingPID = LoadShaders(lighting);

   std::vector<std::string> menu(2, "");
   menu[0] = "shaders/menu.vs";
   menu[1] = "shaders/menu.fs";
   menuPID = LoadShaders(menu);

   glGenBuffers(1, &inBlock);
   glBindBuffer(GL_UNIFORM_BUFFER, inBlock);
   glBufferData(GL_UNIFORM_BUFFER, 272, NULL, GL_STATIC_DRAW);
   glBindBufferBase(GL_UNIFORM_BUFFER, 0, inBlock);

   glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLfloat), &farPlane);
   glBufferSubData(GL_UNIFORM_BUFFER, 4, sizeof(GLfloat), &gamma);
   glBufferSubData(GL_UNIFORM_BUFFER, 8, sizeof(GLfloat), &samples);
   glBufferSubData(GL_UNIFORM_BUFFER, 12, sizeof(GLfloat), &shadows);

   glBindBuffer(GL_UNIFORM_BUFFER, 0);

   glGenBuffers(1, &lightBlock);
   glBindBuffer(GL_UNIFORM_BUFFER, lightBlock);
   glBufferData(GL_UNIFORM_BUFFER, 240, NULL, GL_STATIC_DRAW);
   glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightBlock);
   glBindBuffer(GL_UNIFORM_BUFFER, 0);

   prepGBuffer();
}



void RenderSystem::configure(ex::EventManager& evnM) {
   evnM.subscribe<GenBuffers>(*this);
   evnM.subscribe<PrepShadowMap>(*this);
   evnM.subscribe<SetLights>(*this);
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
      // Generates the shadow cubemaps for each light
      if (shadows)
         genShadowMap(entM);

      // Generates the gBuffer
      genGBuffer(entM);

      // Calculates lighting based on the textures from gBuffer (deferred lighting)
      deferredRender(entM);
   }



   //Renders Menus
   entM.each<Renderable, MenuID, Material>([this, &entM, &currScrn](ex::Entity entity, Renderable& mesh, MenuID& menu, Material& mat) {
      if (currScrn == menu.id) {
         glUseProgram(menuPID);

         //Passes the type of render for the menu shader (could just split shaders)
         ex::ComponentHandle<Font> font = entity.component<Font>();
         if (font)
            glUniform1i(glGetUniformLocation(menuPID, "renderType"), 0);
         else if (mat.texID != 0)
            glUniform1i(glGetUniformLocation(menuPID, "renderType"), 1);
         else
            glUniform1i(glGetUniformLocation(menuPID, "renderType"), 2);

         // Reenables cursor for the menu
         drawScene(mesh, mat, menuPID, entM, entity);
      }
   });
}







void RenderSystem::receive(const GenBuffers& gen) {

   // Need to figure way to remove unused buffers (maybe, does use same ids...)
   genBuffers(gen.entity, gen.verts, gen.norms, gen.uvs);

}






void RenderSystem::receive(const PrepShadowMap& sMap) {

   glBindTexture(GL_TEXTURE_CUBE_MAP, sMap.shadow->depthMap);
   for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}


void RenderSystem::receive(const SetLights& sl) {
   addLight(sl.entM);
}







void RenderSystem::prepGBuffer() {

   glGenFramebuffers(1, &gBuffer);
   glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

   // position color buffer
   glGenTextures(1, &gPosition);
   glBindTexture(GL_TEXTURE_2D, gPosition);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, winWidth, winHeight, 0, GL_RGB, GL_FLOAT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
   // normal color buffer
   glGenTextures(1, &gNormal);
   glBindTexture(GL_TEXTURE_2D, gNormal);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, winWidth, winHeight, 0, GL_RGB, GL_FLOAT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
   // color + specular color buffer
   glGenTextures(1, &gAlbedoSpec);
   glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, winWidth, winHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, winWidth, winHeight);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

   // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
   //      std::cout << "Framebuffer not complete!" << std::endl;

   glBindFramebuffer(GL_FRAMEBUFFER, 0);


   // Pass quad to buffers (this could be linked up with the menu quad)
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









void RenderSystem::genShadowMap(ex::EntityManager& entM) {

   entM.each<Light, Position, Shadow>([this, &entM](ex::Entity eNull, Light& lNull, Position& light, Shadow& shadow) {
      glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)shadowWidth / (float)shadowHeight, nearPlane, farPlane);
      std::vector<glm::mat4> shadowTransforms;
      shadowTransforms.push_back(shadowProj * glm::lookAt(light.pos, light.pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
      shadowTransforms.push_back(shadowProj * glm::lookAt(light.pos, light.pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
      shadowTransforms.push_back(shadowProj * glm::lookAt(light.pos, light.pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
      shadowTransforms.push_back(shadowProj * glm::lookAt(light.pos, light.pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
      shadowTransforms.push_back(shadowProj * glm::lookAt(light.pos, light.pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
      shadowTransforms.push_back(shadowProj * glm::lookAt(light.pos, light.pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

      glViewport(0, 0, shadowWidth, shadowHeight);
      glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow.depthMap, 0);
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
      glUniform3fv(glGetUniformLocation(shadowPID, "lightPos"), 1, &(light.pos[0]));

      //Passes camera and model matrix, and then renders each object
      entM.each<Renderable, Material, Level>([this, &entM](ex::Entity entity, Renderable &mesh, Material& mat, Level& null) {
         drawScene(mesh, mat, shadowPID, entM, entity);
      });
   });
}





void RenderSystem::genGBuffer(ex::EntityManager& entM) {

   glViewport(0, 0, winWidth, winHeight);
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
   entM.each<Renderable, Material, Level>([this, &entM](ex::Entity entity, Renderable &mesh, Material& mat, Level& null) {
      // Pass material shininess to shader
      glUniform1f(glGetUniformLocation(deferPID, "material.shininess"), 20.0);

      drawScene(mesh, mat, deferPID, entM, entity);
   });

}




void RenderSystem::deferredRender(ex::EntityManager& entM) {

   // Reset everything for proper rendering
   glEnable(GL_BLEND);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glUseProgram(lightingPID);

   // Pass gbuffer textures to shader
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, gPosition);
   glUniform1i(glGetUniformLocation(lightingPID, "gPosition"), 0);

   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, gNormal);
   glUniform1i(glGetUniformLocation(lightingPID, "gNormal"), 1);

   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
   glUniform1i(glGetUniformLocation(lightingPID, "gAlbedoSpec"), 2);

   //This is a bodge, figure out how to fix!
   GLuint iLight = 0;
   entM.each<Light, Shadow>([this, &iLight](ex::Entity entity, Light& null, Shadow& shadow) {
      std::stringstream dmName;
      dmName << "light2[" << iLight++ << "].depthMap";

      glUniform1i(glGetUniformLocation(lightingPID, &(dmName.str())[0]), 2 + iLight);

      glActiveTexture(GL_TEXTURE2 + iLight);
      glBindTexture(GL_TEXTURE_CUBE_MAP, shadow.depthMap);
   });

   glBindBuffer(GL_UNIFORM_BUFFER, inBlock);
   entM.each<Camera, Position>([this](ex::Entity ent, Camera& cam, Position& pos) {
      //glUniform3fv(glGetUniformLocation(lightingPID, "viewPos"), 1, &(pos.pos[0]));
      glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec4), &(pos.pos[0]));
   });
   glBindBuffer(GL_UNIFORM_BUFFER, 0);

   // Draw quad
   glBindVertexArray(quadVAO);
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
   glBindVertexArray(0);

   // Pull the depth buffer from the gBuffer and write it to the default buffer
   glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
   glBlitFramebuffer(0, 0, winWidth, winHeight, 0, 0, winWidth, winHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

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










void RenderSystem::drawScene(Renderable& mesh, Material& mat, GLuint prog, ex::EntityManager& entM, ex::Entity& entity) {

   //Rebind the objects VAO
   glBindVertexArray(mesh.VAO);

   //Send object's model matrix
   glUniformMatrix4fv(glGetUniformLocation(prog, "model"), 1, GL_FALSE, &mesh.modelMat[0][0]);
   // Passes colour of font to graphic card
   glUniform3fv(glGetUniformLocation(prog, "colour"), 1, &(mat.colour[0]));

   //Resets the texture and binds correct texture
   if (mat.texID != 0) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mat.texID);
   }

   //Render the object
   glDrawElements(GL_TRIANGLES, mesh.indSize, GL_UNSIGNED_SHORT, (void*)0);

   //Unbind the VAO for the next object
   glBindVertexArray(0);

}












void RenderSystem::addLight(ex::EntityManager& entM) {

   glBindBuffer(GL_UNIFORM_BUFFER, lightBlock);

   //Runs over all light entities to send the information to the buffer
   GLuint iNum = 0, offset = 16;
   entM.each<Light, Position>([this, &iNum, &offset]
                                     (ex::Entity entity, Light& l, Position& p) {

      glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec4), &(p.pos[0]));
      glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, sizeof(glm::vec4), &(l.ambient[0]));
      glBufferSubData(GL_UNIFORM_BUFFER, offset + 32, sizeof(glm::vec4), &(l.diffuse[0]));
      glBufferSubData(GL_UNIFORM_BUFFER, offset + 48, sizeof(glm::vec4), &(l.specular[0]));
      glBufferSubData(GL_UNIFORM_BUFFER, offset + 64, sizeof(GLfloat), &l.linear);
      glBufferSubData(GL_UNIFORM_BUFFER, offset + 68, sizeof(GLfloat), &l.quad);

      offset += 80;
      iNum++;

   });

   //Passes the number of lights to the buffer
   glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLint), &iNum);

   glBindBuffer(GL_UNIFORM_BUFFER, 0);

}
