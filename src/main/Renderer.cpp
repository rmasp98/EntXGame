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

   //Generates a VAO for each renderable entity (should also be added to an event create new object)
   entM.each<Renderable, Shader>([this](ex::Entity entity, Renderable &mesh, Shader& pID) {
      genBuffers(entity, mesh, pID);
   });
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
      if (currScrn == 10)
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      else
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

      //Pass the lights to graphics card
      addLight(entM);

      //Passes camera and model matrix, and then renders each object
      entM.each<Renderable, Shader, Level>([this, &entM](ex::Entity entity, Renderable &mesh, Shader& pID, Level& null) {
         entM.each<Camera>([this, &mesh, &pID](ex::Entity ent, Camera& cam) {
            //Send camera information to the buffer
            glm::mat4 camView = cam.projection * cam.view * mesh.modelMat;
            glUniformMatrix4fv(glGetUniformLocation(pID.progID, "camView"), 1, GL_FALSE, &camView[0][0]);
         });

         drawScene(mesh, pID, entM, entity);
      });

      entM.each<Renderable, Shader, MenuID>([this, &entM, &currScrn](ex::Entity entity, Renderable& mesh, Shader& pID, MenuID& menu) {
         if (currScrn == menu.id)
            drawScene(mesh, pID, entM, entity);
      });
   } else {
      // Reenables cursor for the menu
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

      //Passes camera and model matrix, and then renders each object
      entM.each<Renderable, Shader, MenuID>([this, &entM, &currScrn](ex::Entity entity, Renderable& mesh, Shader& pID, MenuID& menu) {
         if (currScrn == menu.id)
            drawScene(mesh, pID, entM, entity);
      });
   }



   //Reset the VAO
   glBindVertexArray(0);
}



//This creates the VAOs, which are later used to quick switch between objects to render
void RenderSystem::genBuffers(ex::Entity& ent, Renderable& eVecs, Shader& prog) {

   std::vector<unsigned short> inds;
   std::vector<glm::vec2> uvsInds;
   std::vector<glm::vec3> vertInds, normInds;

   glUseProgram(prog.progID);

   //This performs VBO indexing
   indexVBO(eVecs.verts, eVecs.uvs, eVecs.norms, inds, vertInds, uvsInds, normInds);
   eVecs.indSize = inds.size();

   //Generate and bind VAO
   glGenVertexArrays(1, &eVecs.VAO);
   glBindVertexArray(eVecs.VAO);

   //Bind vertices to shader
   glGenBuffers(1, &eVecs.vertID);
   glBindBuffer(GL_ARRAY_BUFFER, eVecs.vertID);
   glBufferData(GL_ARRAY_BUFFER, vertInds.size() * sizeof(glm::vec3), &vertInds[0], GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

   //Bind texture locations to shader
   glGenBuffers(1, &eVecs.uvID);
   glBindBuffer(GL_ARRAY_BUFFER, eVecs.uvID);
   glBufferData(GL_ARRAY_BUFFER, uvsInds.size() * sizeof(glm::vec2), &uvsInds[0], GL_STATIC_DRAW);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

   //Bind normals to shader
   glGenBuffers(1, &eVecs.normID);
   glBindBuffer(GL_ARRAY_BUFFER, eVecs.normID);
   glBufferData(GL_ARRAY_BUFFER, normInds.size() * sizeof(glm::vec3), &normInds[0], GL_STATIC_DRAW);
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

   //Bind VBO indices to shader
   glGenBuffers(1, &eVecs.indID);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eVecs.indID);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(unsigned short), &inds[0] , GL_STATIC_DRAW);

   //Unbind VAO
   glBindVertexArray(0);

}






void RenderSystem::drawScene(Renderable& mesh, Shader& prog, ex::EntityManager& entM, ex::Entity& entity) {

   glUseProgram(prog.progID);

   //Rebind the objects VAO
   glBindVertexArray(mesh.VAO);

   // Passes colour of font to graphic card
   ex::ComponentHandle<Font> font = entity.component<Font>();
   if (font)
      glUniform1i(glGetUniformLocation(prog.progID, "renderType"), 0);
   else if (mesh.texID != 0)
      glUniform1i(glGetUniformLocation(prog.progID, "renderType"), 1);
   else
      glUniform1i(glGetUniformLocation(prog.progID, "renderType"), 2);

   //Send object's model matrix
   glUniformMatrix4fv(glGetUniformLocation(prog.progID, "model"), 1, GL_FALSE, &mesh.modelMat[0][0]);
   glUniform3fv(glGetUniformLocation(prog.progID, "colour"), 1, &(mesh.colour[0]));

   //Resets the texture and binds correct texture
   if (mesh.texID != 0) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mesh.texID);

      glUniform1f(glGetUniformLocation(prog.progID, "material.shininess"), 1.0);
   }

   //Render the object
   glDrawElements(GL_TRIANGLES, mesh.indSize, GL_UNSIGNED_SHORT, (void*)0);

   //Unbind the VAO for the next object
   glBindVertexArray(0);
}





void RenderSystem::addLight(ex::EntityManager& entM) {

   //Runs over all light entities to send the information to the buffer
   GLuint iNum = 0, pID = 0;
   entM.each<Light, Position, Shader>([this, &iNum, &pID]
                                     (ex::Entity entity, Light& l, Position& p, Shader& s) {

      pID = s.progID;
      glUseProgram(pID);

      //Creates a custom name to pass the lights into an array
      std::stringstream bName;
      bName << "light[" << iNum++ << "].";

      glUniform3f(glGetUniformLocation(s.progID, &(bName.str() + "pos")[0]),
                                       p.pos.x, p.pos.y, p.pos.z);

      glUniform3f(glGetUniformLocation(s.progID, &(bName.str() + "ambient")[0]),
                                       l.ambient.x, l.ambient.y, l.ambient.z);

      glUniform3f(glGetUniformLocation(s.progID, &(bName.str() + "diffuse")[0]),
                                       l.diffuse.x, l.diffuse.y, l.diffuse.z);

      glUniform3f(glGetUniformLocation(s.progID, &(bName.str() + "specular")[0]),
                                       l.specular.x, l.specular.y, l.specular.z);

      glUniform1f(glGetUniformLocation(s.progID, &(bName.str() + "linear")[0]), l.linear);
      glUniform1f(glGetUniformLocation(s.progID, &(bName.str() + "quad")[0]), l.quad);
   });

   //Passes the number of lights to the buffer
   glUniform1i(glGetUniformLocation(pID, "lightNum"), iNum);

}
