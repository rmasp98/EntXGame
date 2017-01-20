#include "main/BuildRoom.hpp"





RoomSystem::RoomSystem(ex::EntityManager& entM) {

   ex::Entity entity = entM.create();

   std::vector< glm::tvec2<GLint> > blocks = createBlocks(100);
   entity.assign<Room>(blocks);

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
            bPos[iBlock] += glm::tvec2<GLint>(pmDir, 0);
         else
            bPos[iBlock] += glm::tvec2<GLint>(0, pmDir);


         flag = false;
         for (GLint jBlock=0; jBlock<iBlock; jBlock++) {
            if ((bPos[jBlock].x == bPos[iBlock].x) && (bPos[jBlock].y == bPos[iBlock].y))
               flag = true;
         }
      }
   }

   return bPos;
}

