////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                BuildRoom.cpp                                       //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#include "main/BuildRoom.hpp"


RoomSystem::RoomSystem(ex::EntityManager& entM) {

   //Create the room entity and assign it as a room
   ex::Entity entity = entM.create();
   entity.assign<Level>();

   entity.assign<Room>();
   ex::ComponentHandle<Room> roomC = entity.component<Room>();

   //Creates the base of the room. This is where the game logic should be!
   //roomC->blocks = createBlocks(100);
   roomC->blocks = createBlocks();

   //Get shader from the objectGen entities
   GLuint pID;
   entM.each<Shader>([this, &pID](ex::Entity ent, Shader& shader) {
      pID = shader.progID;
   });
   entity.assign<Shader>(pID);

   //Determine the boundary around the blocks
   createBound(roomC);

   //Use everything to generate the vertices, norms and UVs
   buildRoom(entM, entity, roomC, pID);

}





void RoomSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {

}


std::vector< glm::tvec2<GLint> > RoomSystem::createBlocks() {

   std::vector< glm::tvec2<GLint> > bPos(30, glm::tvec2<GLint>(0));

   bPos[0] = glm::tvec2<GLint>(0, 0);
   bPos[1] = glm::tvec2<GLint>(-2, 0);
   bPos[2] = glm::tvec2<GLint>(-3, 0);
   bPos[3] = glm::tvec2<GLint>(-4, 0);
   bPos[4] = glm::tvec2<GLint>(-5, 0);

   bPos[5] = glm::tvec2<GLint>(0, 1);
   bPos[6] = glm::tvec2<GLint>(-4, 1);
   bPos[7] = glm::tvec2<GLint>(-5, 1);

   bPos[8] = glm::tvec2<GLint>(1, 2);
   bPos[9] = glm::tvec2<GLint>(0, 2);
   bPos[10] = glm::tvec2<GLint>(-1, 2);
   bPos[11] = glm::tvec2<GLint>(-3, 2);
   bPos[12] = glm::tvec2<GLint>(-4, 2);
   bPos[13] = glm::tvec2<GLint>(-5, 2);

   bPos[14] = glm::tvec2<GLint>(1, 3);
   bPos[15] = glm::tvec2<GLint>(0, 3);
   bPos[16] = glm::tvec2<GLint>(-1, 3);
   bPos[17] = glm::tvec2<GLint>(-2, 3);
   bPos[18] = glm::tvec2<GLint>(-3, 3);
   bPos[19] = glm::tvec2<GLint>(-4, 3);
   bPos[20] = glm::tvec2<GLint>(-5, 3);

   bPos[21] = glm::tvec2<GLint>(1, 4);
   bPos[22] = glm::tvec2<GLint>(0, 4);
   bPos[23] = glm::tvec2<GLint>(-1, 4);
   bPos[24] = glm::tvec2<GLint>(-2, 4);
   bPos[25] = glm::tvec2<GLint>(-3, 4);
   bPos[26] = glm::tvec2<GLint>(-4, 4);
   bPos[27] = glm::tvec2<GLint>(-5, 4);

   bPos[28] = glm::tvec2<GLint>(-4, 5);
   bPos[29] = glm::tvec2<GLint>(-5, 5);

   return bPos;

}


std::vector< glm::tvec2<GLint> > RoomSystem::createBlocks(GLint numBlocks) {

   std::vector< glm::tvec2<GLint> > bPos(numBlocks, glm::tvec2<GLint>(0));

   //Randomly seed from system time
   srand(time(NULL));

   //Loops over input number of blocks until all blocks created
   for (int iBlock=1; iBlock<numBlocks; iBlock++) {
      //Flag to determine if block exists already
      bool flag = true;
      GLint block = iBlock;
      while (flag) {
         //If Block already exists then it will try with previous block
         if (block < 1)
            block = iBlock;
         else
            block--;

         //Randomly choose one of four surrounding blocks to populate
         GLint dir = rand() % 4, pmDir = 2*(dir%2) - 1;
         if (dir < 2)
            bPos[iBlock] = bPos[block] + glm::tvec2<GLint>(pmDir, 0);
         else
            bPos[iBlock] = bPos[block] + glm::tvec2<GLint>(0, pmDir);

         //Assumes block doesn't already exist unless found
         flag = false;
         for (GLint jBlock=0; jBlock<iBlock; jBlock++) {
            //Checks position proposed block with all other blocks
            if ((bPos[jBlock].x == bPos[iBlock].x) && (bPos[jBlock].y == bPos[iBlock].y))
               flag = true;
         }
      }
   }

   return bPos;
}






void RoomSystem::createBound(ex::ComponentHandle<Room>& rC) {

   //Runs over every block to see if it has a boundary
   GLint count = 0;
   for (GLuint iBlock=0; iBlock<rC->blocks.size(); iBlock++) {
      bool xm=true, xp=true, ym=true, yp=true;
      for (GLuint jBlock=0; jBlock<rC->blocks.size(); jBlock++) {
         //Checks if a block exists in each direction
         if ((rC->blocks[iBlock][0] + 1 == rC->blocks[jBlock][0]) && (rC->blocks[iBlock][1] == rC->blocks[jBlock][1]))
            xp = false;
         else if ((rC->blocks[iBlock][0] - 1 == rC->blocks[jBlock][0]) && (rC->blocks[iBlock][1] == rC->blocks[jBlock][1]))
            xm = false;
         else if ((rC->blocks[iBlock][0] == rC->blocks[jBlock][0]) && (rC->blocks[iBlock][1] + 1 == rC->blocks[jBlock][1]))
            yp = false;
         else if ((rC->blocks[iBlock][0] == rC->blocks[jBlock][0]) && (rC->blocks[iBlock][1] - 1 == rC->blocks[jBlock][1]))
            ym = false;
      }

      //TODO: Can use when three are true to place destination markers (will need to make some way of forcing possible solution)
      //If the isn't a neighbouring block, then add a boundary. The bools are x/y and plus/minus
      if (xp) {
         rC->bound.push_back(std::vector<GLfloat>(4,0));
         rC->bound[count][0] = rC->blocks[iBlock][0] + 0.5; rC->bound[count][2] = rC->blocks[iBlock][0] + 0.5;
         rC->bound[count][1] = rC->blocks[iBlock][1] - 0.5; rC->bound[count][3] = rC->blocks[iBlock][1] + 0.5;
         count++;

         rC->norms.push_back(glm::vec3(-1.0, 0.0, 0.0));
      }

      if (xm) {
         rC->bound.push_back(std::vector<GLfloat>(4,0));
         rC->bound[count][0] = rC->blocks[iBlock][0] - 0.5; rC->bound[count][2] = rC->blocks[iBlock][0] - 0.5;
         rC->bound[count][1] = rC->blocks[iBlock][1] + 0.5; rC->bound[count][3] = rC->blocks[iBlock][1] - 0.5;
         count++;

         rC->norms.push_back(glm::vec3(1.0, 0.0, 0.0));
      }

      if (yp) {
         rC->bound.push_back(std::vector<GLfloat>(4,0));
         rC->bound[count][0] = rC->blocks[iBlock][0] + 0.5; rC->bound[count][2] = rC->blocks[iBlock][0] - 0.5;
         rC->bound[count][1] = rC->blocks[iBlock][1] + 0.5; rC->bound[count][3] = rC->blocks[iBlock][1] + 0.5;
         count++;

         rC->norms.push_back(glm::vec3(0.0, 0.0, -1.0));
      }

      if (ym) {
         rC->bound.push_back(std::vector<GLfloat>(4,0));
         rC->bound[count][0] = rC->blocks[iBlock][0] - 0.5; rC->bound[count][2] = rC->blocks[iBlock][0] + 0.5;
         rC->bound[count][1] = rC->blocks[iBlock][1] - 0.5; rC->bound[count][3] = rC->blocks[iBlock][1] - 0.5;
         count++;

         rC->norms.push_back(glm::vec3(0.0, 0.0, 1.0));
      }
   }

   //TODO: Find a better fix
   //Scale the boundary to the real world size
   for (GLuint i=0; i<rC->bound.size(); i++) {
      for (GLuint j=0; j<rC->bound[i].size(); j++)
         rC->bound[i][j] = rC->bound[i][j] * 2.0;
   }
}









void RoomSystem::buildRoom(ex::EntityManager& entM, ex::Entity& ent, ex::ComponentHandle<Room>& rC, GLuint pID) {

   //Scales floor and roof. Should fix
   GLfloat rScale = 2;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Build the walls

   //The order for converting xy to xyz verts
   GLint xyInd[6] = {0, 2, 0, 2, 0, 2};
   //The value and order for the z components
   GLfloat zVal[6] = {0.0, 0.0, 5.0, 5.0, 5.0, 0.0};
   //Universal UV coordinates. Will change when new textures added
   glm::vec2 uvs[6] = {glm::vec2(0.748573, 0.501077),
                       glm::vec2(0.748573, 0.750412),
                       glm::vec2(0.999110, 0.501077),
                       glm::vec2(0.999110, 0.501077),
                       glm::vec2(0.748573, 0.750412),
                       glm::vec2(0.999455, 0.750380)};

   std::vector<glm::vec3> roomVerts(6*rC->bound.size(), glm::vec3(0)), roomNorms(6*rC->bound.size(), glm::vec3(0));
   std::vector<glm::vec2> roomUVs(6*rC->bound.size(), glm::vec2(0));
   //Runs over every boundary and converts it into faces of two triangles
   for (GLuint iRoom=0; iRoom<rC->bound.size(); iRoom++) {
      for (GLuint jRoom=0; jRoom<6; jRoom++) {
         roomVerts[6*iRoom+jRoom].x = rC->bound[iRoom][xyInd[jRoom]];//*rScale;
         roomVerts[6*iRoom+jRoom].y = zVal[jRoom];
         roomVerts[6*iRoom+jRoom].z = rC->bound[iRoom][xyInd[jRoom]+1];//*rScale;

         //Norms are same for the entire face and UVs are currently all the same
         roomNorms[6*iRoom+jRoom] = rC->norms[iRoom];
         roomUVs[6*iRoom+jRoom] = uvs[jRoom];
      }
   }


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Build the floor and roof

   std::vector<glm::vec3> floorVerts(6*rC->blocks.size(), glm::vec3(0)), roofVerts(6*rC->blocks.size(), glm::vec3(0));
   std::vector<glm::vec3> floorNorm(6*rC->blocks.size(), glm::vec3(0,1,0)), roofNorm(6*rC->blocks.size(), glm::vec3(0,-1,0));
   std::vector<glm::vec2> floorUV(6*rC->blocks.size(), glm::vec2(0)), roofUV(6*rC->blocks.size(), glm::vec2(0));

   //Creates a relative shift for creating the face from two triangles
   GLfloat xShift[6] = {-0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
   GLfloat yShift[6] = {-0.5, 0.5, -0.5, -0.5, 0.5, 0.5};

   //Runs over every block to create the floors and roof faces
   for (GLuint i=0; i<rC->blocks.size(); i++) {
      for (GLuint j=0; j<6; j++) {
         floorVerts[6*i+j].x = (rC->blocks[i][0] + xShift[j]) * rScale;
         floorVerts[6*i+j].y = 0.0;
         floorVerts[6*i+j].z = (rC->blocks[i][1] + yShift[j]) * rScale;
         floorUV[6*i+j] = uvs[j];
      }

      //Every 15 blocks (probably should be configurable), it creates a light
      if ((i % 10) == 0) {
         ex::Entity lightEnt = entM.create();
         glm::vec3 amb(0.1f), diff(0.5f), spec(0.05f), pos(rC->blocks[i][0]*rScale, 4.5, rC->blocks[i][1]*rScale);

         lightEnt.assign<Light>(amb, diff, spec, 0.07, 0.017);
         lightEnt.assign<Position>(pos, glm::vec3(0.0f));
         lightEnt.assign<Shader>(pID);
      }
   }

   //Adds the floor to the boundary verts, norms and UVs
   roomVerts.insert(roomVerts.end(), floorVerts.begin(), floorVerts.end());
   roomUVs.insert(roomUVs.end(), floorUV.begin(), floorUV.end());
   roomNorms.insert(roomNorms.end(), floorNorm.begin(), floorNorm.end());

   //Does the same as floor for the roof
   for (GLuint i=0; i<rC->blocks.size(); i++) {
      for (GLuint j=0; j<6; j++) {
         roofVerts[6*i+j].x = (rC->blocks[i][0] + xShift[5-j]) * rScale;
         roofVerts[6*i+j].y = 5.0;
         roofVerts[6*i+j].z = (rC->blocks[i][1] + yShift[5-j]) * rScale;
         roofUV[6*i+j] = uvs[j];
      }
   }

   roomVerts.insert(roomVerts.end(), roofVerts.begin(), roofVerts.end());
   roomUVs.insert(roomUVs.end(), roofUV.begin(), roofUV.end());
   roomNorms.insert(roomNorms.end(), roofNorm.begin(), roofNorm.end());

   //Loads the texture for the walls. The file name will need to be loaded from a config file
   GLint texID;
   std::string texFile = "assets/roomUV.DDS";
   if (texFile != "") {
      texID = (loadDDS(texFile.c_str()));
   } else {
      std::cerr << "Failed to load UV map!\n";
      exit(EXIT_FAILURE);
   }

   //Assign all the verts, norms, UVs and texture ID to the room entity
   ent.assign<Renderable>(roomVerts, roomNorms, roomUVs, texID);

}
