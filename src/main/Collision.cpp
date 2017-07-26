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

   GLuint currScrn;
   entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
      currScrn = screen.id;
   });


   //collision should only check things where tempPos is different to pos
   if (currScrn == 10) {
      //Passes the room in to check if entity has collided against wall
      entM.each<Room>([this, &entM](ex::Entity roomEnt, Room& room) {
      //Checks every for collision for any collidable entities
         entM.each<Collidable, Position>([this, &room, &entM](ex::Entity entity1, Collidable& coll, Position& pos1) {
            // If entity has moved
            if (glm::distance(pos1.pos, pos1.tempPos) > 1e-3) {
               entM.each<Collidable, Position, Push>([this, &entity1, &pos1](ex::Entity entity2, Collidable& coll, Position& pos2, Push& push) {
                  if (entity1 != entity2) {
                     if (testCollision(pos1, pos2)) {
                        objectCollision(pos1, pos2, push, entity1);
                     } else
                        push.count = 0;
                  }
               });

               wallCollision(pos1, room);
            }
         });
      });

      entM.each<Collidable, Position, Renderable, Push>([this, &entM](ex::Entity entity1, Collidable& coll, Position& pos1, Renderable& mesh, Push& push) {
         bool isDone = false;
         entM.each<Goal, Position>([this, &pos1, &mesh, &isDone, &push](ex::Entity entity2, Goal& goal, Position& pos2) {
            if (testCollision(pos1, pos2)) {
               mesh.colour = glm::vec3(1.0f, 0.5f, 0.5f);
               isDone = true;
               push.delay = push.delLong;
            }

            if (!isDone) {
               mesh.colour = glm::vec3(1.0f, 1.0f, 1.0f);
               push.delay = push.delShort;
            }

         });
      });

      evnM.emit<UpdatePos>(entM);
   }
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
   //Should check these values are initialised
   if (std::abs(room.norms[bestInd].z) < 1e-3 ) {
      //Then checks which direction it is facing
      if (room.norms[bestInd].x < 0) {
         //Then checks if it is inside or outside the boundary + the objects buffer
         if (pos.tempPos.x > room.bound[bestInd][0] - pos.buffer.x)
            //If it is, move the object back inside the room
            pos.tempPos.x = room.bound[bestInd][0] - pos.buffer.x;
      } else if (room.norms[bestInd].x > 0) {
         if (pos.tempPos.x < room.bound[bestInd][0] + pos.buffer.x)
            pos.tempPos.x = room.bound[bestInd][0] + pos.buffer.x;
      }
   } else if (std::abs(room.norms[bestInd].x) < 1e-3) {
      if (room.norms[bestInd].z < 0) {
         if (pos.tempPos.z > room.bound[bestInd][1] - pos.buffer.z)
            pos.tempPos.z = room.bound[bestInd][1] - pos.buffer.z;
      } else if (room.norms[bestInd].z > 0) {
         if (pos.tempPos.z < room.bound[bestInd][1] + pos.buffer.z)
            pos.tempPos.z = room.bound[bestInd][1] + pos.buffer.z;
      }
   }
}





bool CollisionSystem::testCollision(Position& pos1, Position& pos2) {

   glm::vec3 distVec = glm::vec3(pos1.tempPos.x - pos2.tempPos.x, 0.0f, pos1.tempPos.z - pos2.tempPos.z);
   if ((std::abs(distVec.x) < pos1.buffer.x + pos2.buffer.x) && (std::abs(distVec.z) < pos1.buffer.z + pos2.buffer.z))
      return true;

   return false;
}



void CollisionSystem::objectCollision(Position& pos1, Position& pos2, Push& push, ex::Entity& ent1) {
   glm::vec3 distVec = glm::vec3(pos1.tempPos.x - pos2.tempPos.x, 0.0f, pos1.tempPos.z - pos2.tempPos.z);
   if (std::abs(distVec.x) > std::abs(distVec.z))
      pos1.tempPos.x = pos2.tempPos.x + ((distVec.x > 0) ? 1.0 : -1.0) * (pos1.buffer.x + pos2.buffer.x);
   else
      pos1.tempPos.z = pos2.tempPos.z + ((distVec.z > 0) ? 1.0 : -1.0) * (pos1.buffer.z + pos2.buffer.z);

   // If object is pushable
   ex::ComponentHandle<Camera> cam = ent1.component<Camera>();
   if (cam) {
      push.isPush = true;
      push.pushDir = pos1.tempPos - pos2.tempPos;
   }
}
