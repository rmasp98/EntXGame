#include "main/Collision.hpp"

CollisionSystem::CollisionSystem(ex::EntityManager& entM) {

   entM.each<Camera, Position>([this](ex::Entity entity, Camera &camera, Position& pos) {
      camPos = &pos;
   });

}



void CollisionSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {

   entM.each<Room>([this, &entM](ex::Entity entity, Room& room) {
      entM.each<Collidable>([this, &room](ex::Entity entity, Collidable& coll) {
         wallCollision(entity, room);
      });
   });


}







void CollisionSystem::wallCollision(ex::Entity& ent, Room& room) {

   GLfloat bestD;
   GLuint bestInd;
   for (GLuint iColl=0; iColl<room.bound.size(); iColl++) {
      GLfloat dist = glm::distance(glm::vec2(camPos->pos.x, camPos->pos.z),
                     glm::vec2((room.bound[iColl][0] + room.bound[iColl][2])/2.0,
                               (room.bound[iColl][1] + room.bound[iColl][3])/2.0));

      if (iColl==0 || dist < bestD) {
         bestD = dist;
         bestInd = iColl;
      }
   }

   //This is only a simple detection. Make better!
   bool isIn = true;
   if (std::abs(room.norms[bestInd].z) < 1e-3 ) {
      if (room.norms[bestInd].x < 0) {
         if (camPos->pos.x > room.bound[bestInd][0])
            isIn = false;
      } else if (room.norms[bestInd].x > 0) {
         if (camPos->pos.x < room.bound[bestInd][0])
            isIn = false;
      }
   } else if (std::abs(room.norms[bestInd].x) < 1e-3) {
      if (room.norms[bestInd].z < 0) {
         if (camPos->pos.z > room.bound[bestInd][1])
            isIn = false;
      } else if (room.norms[bestInd].z > 0) {
         if (camPos->pos.z < room.bound[bestInd][1])
            isIn = false;
      }
   } else {
      std::cout << "fail" << std::endl;
   }

   //std::cout << isIn << std::endl;


}
