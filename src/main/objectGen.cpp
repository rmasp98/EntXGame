#include "main/objectGen.hpp"




objGenSystem::objGenSystem(ex::EntityManager& entM) {

   ex::Entity entity = entM.create();

   std::vector<glm::vec3> verts, norms;
   std::vector<glm::vec2> uvs;
   std::string objName = "shaders/statue.obj";
   if (!loadOBJ(objName.c_str(), verts, uvs, norms)) {
      std::cerr << "Failed to load asset!\n";
      exit(EXIT_FAILURE);
   }

   entity.assign<Renderable>(verts, norms, uvs);
   ex::ComponentHandle<Renderable> renderable = entity.component<Renderable>();

   std::string vsName = "shaders/main.vs", fsName = "shaders/main.fs";
   if (vsName != "" && fsName != "")
      renderable->progID = LoadShaders(vsName.c_str(), fsName.c_str());
   else {
      std::cerr << "Failed to load shaders!\n";
      exit(EXIT_FAILURE);
   }


   std::string uvName = "shaders/statueUV.DDS";
   if (uvName != "") {
      renderable->texID = (loadDDS(uvName.c_str()));
   } else {
      std::cerr << "Failed to load UV map!\n";
      exit(EXIT_FAILURE);
   }

   entity = entM.create();
   glm::mat4 projection = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 200.0f);
   glm::mat4 view = glm::lookAt(glm::vec3(0.0, 1.0f, 10.0f), glm::vec3(0.0, 1.0f, 0.0f), glm::vec3(0,1,0));

   entity.assign<Camera>(projection, view);

}





void objGenSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {



}
