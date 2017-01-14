#include "main/renderer.hpp"
#include "common/vboIndexer.hpp"


RenderSystem::RenderSystem(ex::EntityManager &entM) {

   entM.each<Camera>([this](ex::Entity entity, Camera &camera) {
      cam = camera;
   });

   entM.each<Renderable>([this](ex::Entity entity, Renderable &mesh) {
      genBuffers(entity, mesh);
   });

}


void RenderSystem::update(ex::EntityManager &entM, ex::EventManager &evnM, ex::TimeDelta dT) {

   entM.each<Renderable>([this](ex::Entity entity, Renderable &mesh) {
      drawScene(mesh);
   });

   glBindVertexArray(0);
}




void RenderSystem::genBuffers(ex::Entity& ent, Renderable& eVecs) {

   std::vector<unsigned short> inds;
   std::vector<glm::vec2> uvsInds;
   std::vector<glm::vec3> vertInds, normInds;


   indexVBO(eVecs.verts, eVecs.uvs, eVecs.norms, inds, vertInds, uvsInds, normInds);

   //This will probably need to be added to the component
   eVecs.indSize = inds.size();

   glGenVertexArrays(1, &eVecs.VAO);
   glBindVertexArray(eVecs.VAO);

   glUseProgram(eVecs.progID);

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

   // Bind our texture in Texture Unit 0
   glUniform1i(glGetUniformLocation(eVecs.progID, "material.diffuse"), 0);
   glUniform1i(glGetUniformLocation(eVecs.progID, "material.specular"), 0);
   glUniform1f(glGetUniformLocation(eVecs.progID, "material.shininess"), 32.0f);

   glBindVertexArray(0);

}






void RenderSystem::drawScene(Renderable& mesh) {

   glBindVertexArray(mesh.VAO);

   //Generate and send camara matrix
   glm::mat4 camView = cam.projection * cam.camSpec * mesh.modelMat;
   glUniformMatrix4fv(glGetUniformLocation(mesh.progID, "camView"), 1, GL_FALSE, &camView[0][0]);

   //Send model matrix
   glUniformMatrix4fv(glGetUniformLocation(mesh.progID, "model"), 1, GL_FALSE, &mesh.modelMat[0][0]);

   //Resets the texture
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mesh.texID);

   glDrawElements(GL_TRIANGLES, mesh.indSize, GL_UNSIGNED_SHORT, (void*)0);
   glBindVertexArray(0);

}
