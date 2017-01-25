#include "main/ObjectGen.hpp"




ObjectSystem::ObjectSystem(ex::EntityManager& entM) {

   entMan = &entM;

   genLevel();

}





void ObjectSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {



}








void ObjectSystem::genLevel() {

   std::string vsName = "shaders/main.vs", fsName = "shaders/main.fs";
   GLuint pID;
   if (vsName != "" && fsName != "")
      pID = LoadShaders(vsName.c_str(), fsName.c_str());
   else {
      std::cerr << "Failed to load shaders!\n";
      exit(EXIT_FAILURE);
   }

   //Create camera
   ex::Entity entity = entMan->create();
   glm::mat4 projection = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 200.0f);
   glm::mat4 view = glm::lookAt(glm::vec3(0.0, 1.0f, 10.0f), glm::vec3(0.0, 1.0f, 0.0f), glm::vec3(0,1,0));

   entity.assign<Camera>(projection, view);
   entity.assign<Position>(glm::vec3(0.0f, 2.0f, 0.0f));
   entity.assign<Acceleration>(0.2f, 1.0f);
   entity.assign<Jump>(3.0f, -9.8f);
   entity.assign<Direction>(0.1f, glm::vec2(0, 0), glm::vec3(0.0f, 0.0f, 1.0f));

   genObject("shaders/statue.obj", "shaders/statueUV.DDS", pID);

   //Create light
   entity = entMan->create();
   glm::vec3 amb(1.0f), diff(1.0f), spec(1.0f), pos(0.0f, 9.5f, 0.0f);

   entity.assign<Light>(amb, diff, spec);
   entity.assign<Position>(pos);
   entity.assign<Shader>(pID);

}








void ObjectSystem::genObject(std::string objFile, std::string texFile, GLint pID) {

   ex::Entity entity = entMan->create();

   std::vector<glm::vec3> verts, norms;
   std::vector<glm::vec2> uvs;

   if (!loadOBJ(objFile.c_str(), verts, uvs, norms)) {
      std::cerr << "Failed to load asset!\n";
      exit(EXIT_FAILURE);
   }

   GLint texID;
   if (texFile != "") {
      texID = (loadDDS(texFile.c_str()));
   } else {
      std::cerr << "Failed to load UV map!\n";
      exit(EXIT_FAILURE);
   }

   entity.assign<Renderable>(verts, norms, uvs, texID);
   entity.assign<Shader>(pID);
   entity.assign<Position>(glm::vec3(0.0f, 1.0f, 0.0f));

}








