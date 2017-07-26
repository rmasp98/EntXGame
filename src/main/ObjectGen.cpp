////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                ObjectGen.cpp                                       //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/ObjectGen.hpp"




ObjectSystem::ObjectSystem(ex::EntityManager& entM) {

   //Entity manager is used in many functions so stored as local variable
   entMan = &entM;

   //Load the shaders (need to add shader names to config file)
   std::string vsName = "shaders/main.vs", fsName = "shaders/main.fs";
   GLuint pID;
   if (vsName != "" && fsName != "")
      pID = LoadShaders(vsName.c_str(), fsName.c_str());
   else {
      std::cerr << "Failed to load shaders!\n";
      exit(EXIT_FAILURE);
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //Create camera
   ex::Entity entity = entMan->create();
   //Camera perspective: angle of view, xy ratio, minimum distance, maximum distance
   glm::mat4 projection = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 200.0f);
   //Camera View: Camera poistion, facing direction, the up vector (always z up)
   glm::mat4 view = glm::lookAt(glm::vec3(0.0, 1.0f, 10.0f), glm::vec3(0.0, 1.0f, 0.0f), glm::vec3(0,1,0));
   entity.assign<Camera>(projection, view);

   //Set initial position and collision buffer of camera
   entity.assign<Position>(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.5f));
   //Movement: max speed, acceleration
   entity.assign<Acceleration>(5.0f, 30.0f);
   //Jump speed, initial jump speed and acceleration due to gravity
   entity.assign<Jump>(3.0f, -9.8f);
   //Facing direction: mouse speed, initial viewing angle, initial facing vector
   entity.assign<Direction>(0.1f, glm::vec2(0, 0), glm::vec3(0.0f, 0.0f, 1.0f));
   //If object will collide
   entity.assign<Collidable>();

   //Generate first level (should probably move this to update within if statement)
   genLevel(pID);

}





void ObjectSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {



}







//TODO: Currently hardcode a lot of values which should be read from a config file
void ObjectSystem::genLevel(GLuint pID) {

   entMan->each<Camera, Position>([this](ex::Entity roomEnt, Camera& null, Position& pos) {
      pos.pos = glm::vec3(0.0f, 3.0f, 0.0f);
   });

   /////////////////////////////////////////////////////////////////////////////
   //Generate objects in the room (i.e. blocks)
   std::vector<glm::vec3> boxPos(7, glm::vec3(0.0f));
   boxPos[0] = glm::vec3(0.0, 1.0, 4.0);
   boxPos[1] = glm::vec3(-2.0, 1.0, 6.0);
   boxPos[2] = glm::vec3(-4.0, 1.0, 6.0);
   boxPos[3] = glm::vec3(-8.0, 1.0, 6.0);
   boxPos[4] = glm::vec3(-8.0, 1.0, 4.0);
   boxPos[5] = glm::vec3(-8.0, 1.0, 8.0);
   boxPos[6] = glm::vec3(-8.0, 1.0, 0.0);
   genObject("assets/cube.obj", "assets/roomUV.DDS", pID, boxPos);

   //Create Goal lights
   std::vector<glm::vec3> boxGoal(7, glm::vec3(0.0f));
   boxGoal[0] = glm::vec3(0.0, 0.5, 0.0);
   boxGoal[1] = glm::vec3(-2.0, 0.5, 8.0);
   boxGoal[2] = glm::vec3(-6.0, 0.5, 6.0);
   boxGoal[3] = glm::vec3(-8.0, 0.5, 10.0);
   boxGoal[4] = glm::vec3(-8.0, 0.5, 4.0);
   boxGoal[5] = glm::vec3(-10.0, 0.5, 6.0);
   boxGoal[6] = glm::vec3(-4.0, 0.5, 0.0);
   for (GLuint iGoal=0; iGoal<boxGoal.size(); iGoal++) {
      ex::Entity lightEnt = entMan->create();
      glm::vec3 amb(0.0f), diff(1.0f, -1.0f, -1.0f), spec(0.0f, -0.0f, -0.0f);

      lightEnt.assign<Light>(amb, diff, spec, 0.05, 5.0);
      lightEnt.assign<Position>(boxGoal[iGoal], glm::vec3(0.0f));
      lightEnt.assign<Shader>(pID);
      lightEnt.assign<Goal>();
   }
}







//Currently just creating a statue (later will create the blocks)
void ObjectSystem::genObject(std::string objFile, std::string texFile, GLint pID, std::vector<glm::vec3>& pos) {

   std::vector<glm::vec3> verts, norms;
   std::vector<glm::vec2> uvs;

   if (!loadOBJ(objFile.c_str(), verts, uvs, norms)) {
      std::cerr << "Failed to load asset!\n";
      exit(EXIT_FAILURE);
   }

   //Load texture for object
   GLint texID;
   if (texFile != "") {
      texID = (loadDDS(texFile.c_str()));
   } else {
      std::cerr << "Failed to load UV map!\n";
      exit(EXIT_FAILURE);
   }


   for (GLuint iBox=0; iBox < pos.size(); iBox++) {
      ex::Entity entity = entMan->create();

      //Assign all values to the entity
      entity.assign<Level>();
      entity.assign<Renderable>(verts, norms, uvs, texID);
      entity.assign<Shader>(pID);
      entity.assign<Position>(pos[iBox], glm::vec3(1.0f));
      entity.assign<Collidable>();
      entity.assign<Acceleration>(1.5f, 8.0f);
      entity.assign<Push>(10, 30);
   }
}
