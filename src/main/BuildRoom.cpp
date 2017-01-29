#include "main/BuildRoom.hpp"





RoomSystem::RoomSystem(ex::EntityManager& entM) {

   ex::Entity entity = entM.create();
   //roomComps roomC;
   entity.assign<Room>();
   ex::ComponentHandle<Room> roomC = entity.component<Room>();


   roomC->blocks = createBlocks(100);
   //entity.assign<Room>(roomC.blocks);

   GLuint pID;
   entM.each<Shader>([this, &pID](ex::Entity ent, Shader& shader) {
      pID = shader.progID;
   });
   entity.assign<Shader>(pID);

   createBound(roomC);
   buildRoom(entM, entity, roomC, pID);

}





void RoomSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {


}





std::vector< glm::tvec2<GLint> > RoomSystem::createBlocks(GLint numBlocks) {

   std::vector< glm::tvec2<GLint> > bPos(numBlocks, glm::tvec2<GLint>(0));
   srand(time(NULL));

   for (int iBlock=1; iBlock<numBlocks; iBlock++) {

      bool flag = true;
      GLint block = iBlock;
      while (flag) {
         //Need to add a weighting to reduce chance of reoccuring therefore making more corridors than rooms
         if (block < 1)
            block = iBlock;
         else
            block--;

         GLint dir = rand() % 4, pmDir = 2*(dir%2) - 1;
         if (dir < 2)
            bPos[iBlock] = bPos[block] + glm::tvec2<GLint>(pmDir, 0);
         else
            bPos[iBlock] = bPos[block] + glm::tvec2<GLint>(0, pmDir);


         flag = false;
         for (GLint jBlock=0; jBlock<iBlock; jBlock++) {
            if ((bPos[jBlock].x == bPos[iBlock].x) && (bPos[jBlock].y == bPos[iBlock].y))
               flag = true;
         }
      }
   }

   return bPos;
}






void RoomSystem::createBound(ex::ComponentHandle<Room>& rC) {

   GLint count = 0;
   for (GLuint iBlock=0; iBlock<rC->blocks.size(); iBlock++) {
      bool xm=true, xp=true, ym=true, yp=true;
      for (GLuint jBlock=0; jBlock<rC->blocks.size(); jBlock++) {
         if ((rC->blocks[iBlock][0] + 1 == rC->blocks[jBlock][0]) && (rC->blocks[iBlock][1] == rC->blocks[jBlock][1]))
            xp = false;
         else if ((rC->blocks[iBlock][0] - 1 == rC->blocks[jBlock][0]) && (rC->blocks[iBlock][1] == rC->blocks[jBlock][1]))
            xm = false;
         else if ((rC->blocks[iBlock][0] == rC->blocks[jBlock][0]) && (rC->blocks[iBlock][1] + 1 == rC->blocks[jBlock][1]))
            yp = false;
         else if ((rC->blocks[iBlock][0] == rC->blocks[jBlock][0]) && (rC->blocks[iBlock][1] - 1 == rC->blocks[jBlock][1]))
            ym = false;
      }

      //Can use when three are true to place destination markers (will need to make some way of forcing possible solution)
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

   //Find a better fix
   for (GLuint i=0; i<rC->bound.size(); i++) {
      for (GLuint j=0; j<rC->bound[i].size(); j++)
         rC->bound[i][j] = rC->bound[i][j] * 4.0;
   }
}









void RoomSystem::buildRoom(ex::EntityManager& entM, ex::Entity& ent, ex::ComponentHandle<Room>& rC, GLuint pID) {

   //Scales floor and roof. Should fix
   GLfloat rScale = 4;

   //Build the walls
   GLint xyInd[6] = {0, 2, 0, 2, 0, 2};
   GLfloat zVal[6] = {0.0, 0.0, 5.0, 5.0, 5.0, 0.0};
   glm::vec2 uvs[6] = {glm::vec2(0.748573, 0.501077),
                       glm::vec2(0.748573, 0.750412),
                       glm::vec2(0.999110, 0.501077),
                       glm::vec2(0.999110, 0.501077),
                       glm::vec2(0.748573, 0.750412),
                       glm::vec2(0.999455, 0.750380)};

   std::vector<glm::vec3> roomVerts(6*rC->bound.size(), glm::vec3(0)), roomNorms(6*rC->bound.size(), glm::vec3(0));
   std::vector<glm::vec2> roomUVs(6*rC->bound.size(), glm::vec2(0));
   for (GLuint iRoom=0; iRoom<rC->bound.size(); iRoom++) {
      for (GLuint jRoom=0; jRoom<6; jRoom++) {
         roomVerts[6*iRoom+jRoom].x = rC->bound[iRoom][xyInd[jRoom]];//*rScale;
         roomVerts[6*iRoom+jRoom].y = zVal[jRoom];
         roomVerts[6*iRoom+jRoom].z = rC->bound[iRoom][xyInd[jRoom]+1];//*rScale;

         roomNorms[6*iRoom+jRoom] = rC->norms[iRoom];
         roomUVs[6*iRoom+jRoom] = uvs[jRoom];
      }
   }



   //Build the floor and roof
   std::vector<glm::vec3> floorVerts(6*rC->blocks.size(), glm::vec3(0)), roofVerts(6*rC->blocks.size(), glm::vec3(0));
   std::vector<glm::vec3> floorNorm(6*rC->blocks.size(), glm::vec3(0,1,0)), roofNorm(6*rC->blocks.size(), glm::vec3(0,-1,0));
   std::vector<glm::vec2> floorUV(6*rC->blocks.size(), glm::vec2(0)), roofUV(6*rC->blocks.size(), glm::vec2(0));

   GLfloat xShift[6] = {-0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
   GLfloat yShift[6] = {-0.5, 0.5, -0.5, -0.5, 0.5, 0.5};

   for (GLuint i=0; i<rC->blocks.size(); i++) {
      for (GLuint j=0; j<6; j++) {
         floorVerts[6*i+j].x = (rC->blocks[i][0] + xShift[j]) * rScale;
         floorVerts[6*i+j].y = 0.0;
         floorVerts[6*i+j].z = (rC->blocks[i][1] + yShift[j]) * rScale;
         floorUV[6*i+j] = uvs[j];
      }

      if ((i % 15) == 0) {
         ex::Entity lightEnt = entM.create();
         glm::vec3 amb(0.5f), diff(1.0f), spec(0.3f), pos(rC->blocks[i][0]*rScale, 4.5, rC->blocks[i][1]*rScale);

         lightEnt.assign<Light>(amb, diff, spec);
         lightEnt.assign<Position>(pos);
         lightEnt.assign<Shader>(pID);
      }
   }

   roomVerts.insert(roomVerts.end(), floorVerts.begin(), floorVerts.end());
   roomUVs.insert(roomUVs.end(), floorUV.begin(), floorUV.end());
   roomNorms.insert(roomNorms.end(), floorNorm.begin(), floorNorm.end());

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

   GLint texID;
   std::string texFile = "shaders/roomUV.DDS";
   if (texFile != "") {
      texID = (loadDDS(texFile.c_str()));
   } else {
      std::cerr << "Failed to load UV map!\n";
      exit(EXIT_FAILURE);
   }

   ent.assign<Renderable>(roomVerts, roomNorms, roomUVs, texID);

}










