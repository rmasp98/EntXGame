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
   //Generate first level (should probably move this to update within if statement)
   genLevel();

}





void ObjectSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {



}







//TODO: Currently hardcode a lot of values which should be read from a config file
void ObjectSystem::genLevel() {

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
   entity.assign<Position>(glm::vec3(0.0f, 2.0f, 0.0f), 0.5);
   //Movement: max speed, acceleration
   entity.assign<Acceleration>(5.0f, 30.0f);
   //Jump speed, initial jump speed and acceleration due to gravity
   entity.assign<Jump>(3.0f, -9.8f);
   //Facing direction: mouse speed, initial viewing angle, initial facing vector
   entity.assign<Direction>(0.1f, glm::vec2(0, 0), glm::vec3(0.0f, 0.0f, 1.0f));
   //If object will collide
   entity.assign<Collidable>();

   //Generate objects in the room (i.e. blocks)
   genObject("shaders/statue.obj", "shaders/statueUV.DDS", pID);

}







//Currently just creating a statue (later will create the blocks)
void ObjectSystem::genObject(std::string objFile, std::string texFile, GLint pID) {

   ex::Entity entity = entMan->create();

   std::vector<glm::vec3> verts, norms;
   std::vector<glm::vec2> uvs;

   //Loads the verticies, uv coords and normals from an object file
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

   //GLuint texID = SOIL_load_OGL_texture(texFile.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_DDS_LOAD_DIRECT);
   //if( 0 == texID ) {
      //printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
   //}


   //Assign all values to the entity
   entity.assign<Renderable>(verts, norms, uvs, texID);
   entity.assign<Shader>(pID);
   entity.assign<Position>(glm::vec3(1.0f, 1.0f, 0.0f), 0.5f);
   entity.assign<Collidable>();
   entity.assign<Acceleration>(5.0f, 2.0f);
   entity.assign<Push>();

}
