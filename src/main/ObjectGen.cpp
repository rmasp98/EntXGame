////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                ObjectGen.cpp                                       //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/ObjectGen.hpp"




ObjectSystem::ObjectSystem(ex::EntityManager& entM, ex::EventManager& evtM) {

   //TODO need to load all the object information from a file

   //Entity manager is used in many functions so stored as local variable
   entMan = &entM;
   evtMan = &evtM;

   boxScale = 0.95;

   //loadAssImp("assets/Dragon2.3ds");

}





void ObjectSystem::configure(ex::EventManager& evnM) { evnM.subscribe<GenObjects>(*this); }




void ObjectSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {}





void ObjectSystem::receive(const GenObjects& gen) {

   newLevel();

   entMan->each<Screen>([this](ex::Entity roomEnt, Screen& screen) {
      screen.id = 10;
   });
}




void ObjectSystem::newLevel() {

   entMan->each<Level, Camera>([this](ex::Entity entity, Level& lNULL, Camera& cNULL) {
      entity.destroy();
   });

   entMan->each<Level, Goal>([this](ex::Entity entity, Level& lNULL, Goal& gNULL) {
      entity.destroy();
   });

   entMan->each<Level, Push, Position>([this](ex::Entity entity, Level& lNULL, Push& pNULL, Position& posnull) {
      entity.destroy();
   });

   // Generate camera
   genCamera();

   //Generate first level (should probably move this to update within if statement)
   genLevel();

}





//Create camera
void ObjectSystem::genCamera() {

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
   //It belongs to the level
   entity.assign<Level>();

}






//TODO: Currently hardcode a lot of values which should be read from a config file
void ObjectSystem::genLevel() {

   //Generate objects in the room (i.e. blocks)
   std::vector<glm::vec3> boxPos(7, glm::vec3(0.0f));
   boxPos[0] = glm::vec3(0.0, boxScale, 4.0);
   boxPos[1] = glm::vec3(-2.0, boxScale, 6.0);
   boxPos[2] = glm::vec3(-4.0, boxScale, 6.0);
   boxPos[3] = glm::vec3(-8.0, boxScale, 6.0);
   boxPos[4] = glm::vec3(-8.0, boxScale, 4.0);
   boxPos[5] = glm::vec3(-8.0, boxScale, 8.0);
   boxPos[6] = glm::vec3(-8.0, boxScale, 0.0);
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

      lightEnt.assign<Position>(boxGoal[iGoal], glm::vec3(0.0f));
      lightEnt.assign<Goal>();
      lightEnt.assign<Level>();

      // lightEnt.assign<Light>(amb, diff, spec, 0.05, 5.0);
      // lightEnt.assign<Shadow>();
      // evtMan->emit<PrepShadowMap>(lightEnt.component<Shadow>());
   }
}







//Currently just creating a statue (later will create the blocks)
void ObjectSystem::genObject(std::string objFile, std::string texFile, GLint pID, std::vector<glm::vec3>& pos) {

   std::vector<glm::vec3> verts, norms;
   std::vector<unsigned short> inds;
   std::vector<glm::vec2> uvs;

   if (!loadOBJ(objFile.c_str(), verts, uvs, norms)) {
      std::cerr << "Failed to load asset!\n";
      exit(EXIT_FAILURE);
   }

   for (GLuint i=0; i<verts.size(); i++) {
      verts[i] *= boxScale;
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
      entity.assign<Renderable>();
      entity.assign<Material>(texID, 20.0f);
      entity.assign<Position>(pos[iBox], glm::vec3(boxScale));
      entity.assign<Collidable>();
      entity.assign<Acceleration>(1.5f, 8.0f);
      entity.assign<Push>(10, 30);

      evtMan->emit<GenBuffers>(entity, verts, norms, uvs);
   }
}
