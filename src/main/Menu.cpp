////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Menu.cpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Menu.hpp"


MenuSystem::MenuSystem(ex::EntityManager& entM) {

   //Make the menu entities
   genMenu(entM);

}





void MenuSystem::update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) {



}










void MenuSystem::genMenu(ex::EntityManager& entM) {

   //Load the shaders (need to add shader names to config file)
   /*std::string vsName = "shaders/menu.vs", fsName = "shaders/menu.fs";
   GLuint pID;
   if (vsName != "" && fsName != "")
      pID = LoadShaders(vsName.c_str(), fsName.c_str());
   else {
      std::cerr << "Failed to load shaders!\n";
      exit(EXIT_FAILURE);
   }*/


   ex::Entity entity = entM.create();

   std::vector<glm::vec3> verts, norms;
   std::vector<glm::vec2> uvs;


   verts.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
   verts.push_back(glm::vec3(0.5f, -0.5f, 0.0f));
   verts.push_back(glm::vec3(0.0f,  0.5f, 0.0f));

   norms.push_back(glm::vec3(0,0,-1));
   norms.push_back(glm::vec3(0,0,-1));
   norms.push_back(glm::vec3(0,0,-1));

   uvs.push_back(glm::vec2(0));
   uvs.push_back(glm::vec2(0));
   uvs.push_back(glm::vec2(0));

   //GLuint texID = 99;

   //Loads the verticies, uv coords and normals from an object file
   /*std::string objFile = "shaders/statue.obj";
   if (!loadOBJ(objFile.c_str(), verts, uvs, norms)) {
      std::cerr << "Failed to load asset!\n";
      exit(EXIT_FAILURE);
   }*/

   //Load texture for object
   /*GLint texID;
   std::string texFile = "shaders/statueUV.DDS";
   if (texFile != "") {
      texID = (loadDDS(texFile.c_str()));
   } else {
      std::cerr << "Failed to load UV map!\n";
      exit(EXIT_FAILURE);
   }*/

   //Assign all values to the entity
   /*entity.assign<Renderable>(verts, norms, uvs, texID);
   entity.assign<Shader>(pID);
   entity.assign<Position>(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f);*/

}
