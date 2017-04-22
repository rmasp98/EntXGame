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

   bool isMenu;
   entM.each<IsMenu>([this, &isMenu](ex::Entity roomEnt, IsMenu& menu) {
      isMenu = menu.isOn;
   });

   if (!isMenu) {
      //Passes the room in to check if entity has collided against wall
      entM.each<Room>([this, &entM, &evnM](ex::Entity roomEnt, Room& room) {
      //Checks every for collision for any collidable entities
         entM.each<Collidable, Position>([this, &room, &entM, &evnM](ex::Entity entity1, Collidable& coll, Position& pos1) {
            wallCollision(pos1, room);

            bool isSame=false;
            entM.each<Collidable, Position>([this, &entity1, &isSame, &evnM](ex::Entity entity2, Collidable& coll, Position& pos2) {
               if (entity1 == entity2)
                  isSame = true;
               else if (isSame)
                  objectCollision(entity1, entity2, evnM);

            });
         });
      });
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





void CollisionSystem::objectCollision(ex::Entity& ent1, ex::Entity& ent2, ex::EventManager& evM) {

   ex::ComponentHandle<Position> pos1 = ent1.component<Position>();
   ex::ComponentHandle<Position> pos2 = ent2.component<Position>();
   if (pos1 && pos2) {
      glm::vec3 distVec = glm::vec3(pos1->pos.x - pos2->pos.x, 0.0f, pos1->pos.z - pos2->pos.z);
      GLfloat dist = glm::length(distVec);
      if (dist < pos1->buffer + pos2->buffer) {
         pos1->pos = pos2->pos + (pos1->buffer + pos2->buffer)*distVec/dist;

         ex::ComponentHandle<Camera> cam = ent1.component<Camera>();
         ex::ComponentHandle<Push> push = ent2.component<Push>();
         ex::ComponentHandle<Acceleration> vel1 = ent1.component<Acceleration>();
         ex::ComponentHandle<Acceleration> vel2 = ent2.component<Acceleration>();
         if (cam && push && vel1 && vel2) {
            push->isPush = true;
            push->pushDir = pos1->pos - pos2->pos;
            //Vel2 has not been updated yet so will set vel1 to 0
            //vel1->vel = glm::length(vel2->vel) < vel1->maxSpeed ? vel2->vel : vel1->vel;
         }

      }
   } else {
      std::cerr << "Something went very wrong! Collision failed\n";
   }

}
