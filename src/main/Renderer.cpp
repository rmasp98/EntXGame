////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Renderer.cpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Renderer.hpp"




RenderSystem::RenderSystem(ex::EntityManager &entM, GLFWwindow* winIn) {

   window = winIn;
   prepShadowMap();

   //Load the shaders (need to add shader names to config file)
   mainPID = LoadShaders("shaders/main.vs", "shaders/main.fs");
   menuPID = LoadShaders("shaders/menu.vs", "shaders/menu.fs");
   shadowPID = LoadShaders("shaders/shadow.vs", "shaders/shadow.gs", "shaders/shadow.fs");
}


void RenderSystem::configure(ex::EventManager& evnM) { evnM.subscribe<GenBuffers>(*this); }




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

      glm::vec3 lightPos;
      entM.each<Light, Position>([this, &lightPos](ex::Entity entity, Light& l, Position& p) {
         lightPos = p.pos;
      });

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
      glClear(GL_DEPTH_BUFFER_BIT);
      glUseProgram(shadowPID);
      for (GLuint iCube = 0; iCube < 6; ++iCube) {
         std::stringstream matName;
         matName << "shadowMatrices[" << iCube << "].";

         glUniformMatrix4fv(glGetUniformLocation(shadowPID, &(matName.str())[0]), 1, GL_FALSE, &shadowTransforms[iCube][0][0]);
      }

      glUniform1f(glGetUniformLocation(shadowPID, "farPlane"), farPlane);
      glUniform3fv(glGetUniformLocation(shadowPID, "lightPos"), 1, &(lightPos[0]));

      //Passes camera and model matrix, and then renders each object
      entM.each<Renderable, Level>([this, &entM](ex::Entity entity, Renderable &mesh, Level& null) {
         // Pass material shininess to shader
         glUniform1f(glGetUniformLocation(mainPID, "material.shininess"), 20.0);

         drawScene(mesh, shadowPID, entM, entity);
      });



      //Pass the lights to graphics card
      addLight(entM, mainPID);

      // Actually render the scene
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glViewport(0, 0, 960, 540);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glUseProgram(mainPID);

      entM.each<Camera, Position>([this](ex::Entity ent, Camera& cam, Position& pos) {
         //Send camera information to the buffer
         glm::mat4 camView = cam.projection * cam.view;
         glUniformMatrix4fv(glGetUniformLocation(mainPID, "camView"), 1, GL_FALSE, &camView[0][0]);
         glUniform3fv(glGetUniformLocation(mainPID, "viewPos"), 1, &(pos.pos[0]));
      });

      //Passes camera and model matrix, and then renders each object
      entM.each<Renderable, Level>([this, &entM](ex::Entity entity, Renderable &mesh, Level& null) {
         // Pass material shininess to shader
         glUniform1f(glGetUniformLocation(mainPID, "material.shininess"), 20.0);

         drawScene(mesh, mainPID, entM, entity);
      });
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
   genBuffers(gen.entity, gen.shader);

}








void RenderSystem::prepShadowMap() {

   // Create the Frame buffer and cubemap to store the shadowmap
   glGenFramebuffers(1, &depthMapFBO);
   glGenTextures(1, &depthCubemap);

   const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
   glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
   for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

   glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
   glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
   glDrawBuffer(GL_NONE);
   glReadBuffer(GL_NONE);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

}








//This creates the VAOs, which are later used to quick switch between objects to render
void RenderSystem::genBuffers(ex::Entity& ent, GLuint shader) {

   ex::ComponentHandle<Renderable> eVecs = ent.component<Renderable>();

   GLuint pID = 0;
   if (shader == 0)
      pID = mainPID;
   else if (shader == 1)
      pID = menuPID;
   else if (shader == 2)
      pID = shadowPID;

   if (eVecs) {
      std::vector<unsigned short> inds;
      std::vector<glm::vec2> uvsInds;
      std::vector<glm::vec3> vertInds, normInds;

      glUseProgram(pID);

      //This performs VBO indexing
      indexVBO(eVecs->verts, eVecs->uvs, eVecs->norms, inds, vertInds, uvsInds, normInds);
      eVecs->indSize = inds.size();

      //Generate and bind VAO
      glGenVertexArrays(1, &eVecs->VAO);
      glBindVertexArray(eVecs->VAO);

      //Bind vertices to shader
      glGenBuffers(1, &eVecs->vertID);
      glBindBuffer(GL_ARRAY_BUFFER, eVecs->vertID);
      glBufferData(GL_ARRAY_BUFFER, vertInds.size() * sizeof(glm::vec3), &vertInds[0], GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      //Bind texture locations to shader
      glGenBuffers(1, &eVecs->uvID);
      glBindBuffer(GL_ARRAY_BUFFER, eVecs->uvID);
      glBufferData(GL_ARRAY_BUFFER, uvsInds.size() * sizeof(glm::vec2), &uvsInds[0], GL_STATIC_DRAW);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

      //Bind normals to shader
      glGenBuffers(1, &eVecs->normID);
      glBindBuffer(GL_ARRAY_BUFFER, eVecs->normID);
      glBufferData(GL_ARRAY_BUFFER, normInds.size() * sizeof(glm::vec3), &normInds[0], GL_STATIC_DRAW);
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

      //Bind VBO indices to shader
      glGenBuffers(1, &eVecs->indID);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eVecs->indID);
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
   // Passes gamma value to graphics
   glUniform1f(glGetUniformLocation(prog, "gamma"), 2.2);

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

      glUseProgram(pID);

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
