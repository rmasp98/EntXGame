////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Collision.cpp                                       //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Collision.hpp"



CollisionSystem::CollisionSystem() {}




void CollisionSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {

   //Passes the room in to check if entity has collided against wall
   entM.each<Room>([this, &entM](ex::Entity roomEnt, Room& room) {
   //Checks every for collision for any collidable entities
      entM.each<Collidable, Position>([this, &room, &entM](ex::Entity entity1, Collidable& coll, Position& pos1) {
         wallCollision(pos1, room);

         bool isSame=false;
         entM.each<Collidable, Position>([this, &entity1, &pos1, &isSame](ex::Entity entity2, Collidable& coll, Position& pos2) {
            if (entity1 == entity2)
               isSame = true;
            else if (isSame)
               objectCollision(pos1, pos2);

         });
      });
   });
}







void CollisionSystem::wallCollision(Position& pos, Room& room) {

   GLfloat bestD;
   GLuint bestInd;
   //Finds the closest boundary to the entity position
   for (GLuint iColl=0; iColl<room.bound.size(); iColl++) {
      GLfloat dist = glm::distance(glm::vec2(pos.pos.x, pos.pos.z),
                     glm::vec2((room.bound[iColl][0] + room.bound[iColl][2])/2.0,
                               (room.bound[iColl][1] + room.bound[iColl][3])/2.0));

      if (iColl==0 || dist < bestD) {
         bestD = dist;
         bestInd = iColl;
      }
   }

   //TODO: This is only a simple detection. Make better!
   //First check which axis the normal of the boundary is on
   if (std::abs(room.norms[bestInd].z) < 1e-3 ) {
      //Then checks which direction it is facing
      if (room.norms[bestInd].x < 0) {
         //Then checks if it is inside or outside the boundary + the objects buffer
         if (pos.pos.x > room.bound[bestInd][0] - pos.buffer)
            //If it is, move the object back inside the room
            pos.pos.x = room.bound[bestInd][0] - pos.buffer;
      } else if (room.norms[bestInd].x > 0) {
         if (pos.pos.x < room.bound[bestInd][0] + pos.buffer)
            pos.pos.x = room.bound[bestInd][0] + pos.buffer;
      }
   } else if (std::abs(room.norms[bestInd].x) < 1e-3) {
      if (room.norms[bestInd].z < 0) {
         if (pos.pos.z > room.bound[bestInd][1] - pos.buffer)
            pos.pos.z = room.bound[bestInd][1] - pos.buffer;
      } else if (room.norms[bestInd].z > 0) {
         if (pos.pos.z < room.bound[bestInd][1] + pos.buffer)
            pos.pos.z = room.bound[bestInd][1] + pos.buffer;
      }
   }
}





void CollisionSystem::objectCollision(Position& ent1, Position& ent2) {

   GLfloat dist = glm::length(glm::vec2(ent1.pos.x - ent2.pos.x, ent1.pos.z - ent2.pos.z));
   if (dist < ent1.buffer + ent2.buffer) {
      ent1.pos.x = ent2.pos.x + (ent1.buffer + ent2.buffer)*(ent1.pos.x - ent2.pos.x)/dist;
      ent1.pos.z = ent2.pos.z + (ent1.buffer + ent2.buffer)*(ent1.pos.z - ent2.pos.z)/dist;
   }


}
