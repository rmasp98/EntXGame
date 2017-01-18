#include "main/objectGen.hpp"




objGenSystem::objGenSystem(ex::EntityManager& entM) {

   //Create statue
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
   GLuint pID;
   if (vsName != "" && fsName != "")
      pID = LoadShaders(vsName.c_str(), fsName.c_str());
   else {
      std::cerr << "Failed to load shaders!\n";
      exit(EXIT_FAILURE);
   }
   entity.assign<Shader>(pID);



   std::string uvName = "shaders/statueUV.DDS";
   if (uvName != "") {
      renderable->texID = (loadDDS(uvName.c_str()));
   } else {
      std::cerr << "Failed to load UV map!\n";
      exit(EXIT_FAILURE);
   }


   //Create camera
   entity = entM.create();
   glm::mat4 projection = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 200.0f);
   glm::mat4 view = glm::lookAt(glm::vec3(0.0, 1.0f, 10.0f), glm::vec3(0.0, 1.0f, 0.0f), glm::vec3(0,1,0));

   entity.assign<Camera>(projection, view);
   entity.assign<Position>(glm::vec3(0.0f));
   entity.assign<Acceleration>(0.5f, 2.5f);
   entity.assign<Jump>(3.0f, -9.8f);
   entity.assign<Direction>(0.1f, glm::vec2(0, 0), glm::vec3(0.0f, 0.0f, 1.0f));

   //Create light
   entity = entM.create();
   glm::vec3 amb(1.0f), diff(1.0f), spec(1.0f), pos(0.0f, 2.0f, 0.0f);

   entity.assign<Light>(amb, diff, spec);
   entity.assign<Position>(pos);
   entity.assign<Shader>(pID);


}





void objGenSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {



}
