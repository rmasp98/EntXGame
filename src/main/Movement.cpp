////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Movement.cpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Movement.hpp"



MoveSystem::MoveSystem(ex::EntityManager& entM) {

   entM.each<Input>([this](ex::Entity null, Input& tempInput) {
      input = &tempInput;
   });

}





void MoveSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {

   GLuint currScrn;
   entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
      currScrn = screen.id;
   });

   if (currScrn == 10) {
      //Look at mouse movement to determine if facing direction changed
      entM.each<Direction>([this, &dT](ex::Entity null, Direction& face) {
         changeDirection(face, dT);
      });

      //Checks for each controllable entity that moves and moves them (currently only moves camera)
      entM.each<Position, Acceleration>([this, dT](ex::Entity entity, Position& pos, Acceleration& accel) {

         ex::ComponentHandle<Direction> face = entity.component<Direction>();

         if (face)
            moveObject(entity, pos, accel, face.get(), dT);
         else
            moveObject(entity, pos, accel, dT);

      });

      //Assigns the calculated values to the camera
      entM.each<Camera, Position, Direction>([this](ex::Entity entity, Camera& cam, Position& pos, Direction& face) {
         cam.view = lookAt(pos.pos, pos.pos + face.facing, glm::vec3(0,1,0));
      });

      if (input->active & input->keyMap["bev"]) {
         entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
            screen.id = 11;
         });
      }
   }


}





void MoveSystem::changeDirection(Direction& dir, GLfloat dT) {

   //Determines the angle of the camera based on how far the cursor position has changed
   dir.angle.x += dir.mouseSpeed * dT * GLfloat(input->winCen[0] - input->cursor[0]);
   double dTheta = dir.mouseSpeed * dT * GLfloat(input->winCen[1] - input->cursor[1]);

   //Makes sure that camera can't look up or down too far
   if (dir.angle.y + dTheta <= -0.9 * glm::pi<GLfloat>() / 2.0f)
      dir.angle.y = -0.9 * glm::pi<GLfloat>() / 2.0f;
   else if (dir.angle.y + dTheta > 0.9 * glm::pi<GLfloat>() / 2.0f)
      dir.angle.y = 0.9 * glm::pi<GLfloat>() / 2.0f;
   else
      dir.angle.y += dTheta;

   //Generates a vector for the faing direction from the angles
   dir.facing = glm::vec3(cos(dir.angle.y) * sin(dir.angle.x),
                          sin(dir.angle.y),
                          cos(dir.angle.y) * cos(dir.angle.x));

   //Creates the movement (2D) vectors (posible should normalise)
   dir.dir = glm::vec3(dir.facing.x, 0, dir.facing.z);
   dir.right = glm::vec3(dir.facing.z, 0, -dir.facing.x);
}





void MoveSystem::moveObject(ex::Entity& ent, Position& pos, Acceleration& accel, Direction* facing, GLfloat dT) {

   bool isSprint = false;
   GLfloat velMod = 1.0;
   //This then updates position and adds a sprint if shift is pressed
   if ((input->active & input->keyMap["sprint"]) || isSprint)
      velMod = 2.0;

   //Some entities don't have jump so need to account for that
   bool isJump = false;
   ex::ComponentHandle<Jump> jump = ent.component<Jump>();
   if (jump) {
      //If spacebar is pressed initiate jump
      if ((input->active & input->keyMap["jump"]) && (!jump->isJump)) {
         jump->isJump = true; accel.vel.y = jump->jumpSpeed;
      }

      isJump = jump->isJump;
   }

   //If jump is true, keypresses have no affect
   if (isJump) {
      //Updates the y position and velocity and the updates xz position
      pos.pos.y += accel.vel.y * dT;
      accel.vel.y += jump->gravity * dT;
      pos.pos += (accel.vel.z * facing->dir + accel.vel.x * facing->right) * dT * velMod;
   } else {
      //If A/D is pressed, increase x speed up to max speed otherwise decelerate down to 0
      if (input->active & input->keyMap["left"])
         accel.vel.x = std::min(accel.vel.x + accel.accel*dT, accel.maxSpeed);
      else if (input->active & input->keyMap["right"])
         accel.vel.x = std::max(accel.vel.x - accel.accel*dT, -accel.maxSpeed);
      else
         //This ensures that it stops at zero and doesn't accelerate in the opposite direction
         accel.vel.x = accel.vel.x < 0 ? std::min(accel.vel.x + accel.accel*dT, 0.0f)
                                           : std::max(accel.vel.x - accel.accel*dT, 0.0f);

      //Same as above but for z direction
      if (input->active & input->keyMap["forward"])
         accel.vel.z = std::min(accel.vel.z + accel.accel*dT, accel.maxSpeed);
      else if (input->active & input->keyMap["backward"])
         accel.vel.z = std::max(accel.vel.z - accel.accel*dT, -accel.maxSpeed);
      else
         accel.vel.z = accel.vel.z < 0 ? std::min(accel.vel.z + accel.accel*dT, 0.0f)
                                           : std::max(accel.vel.z - accel.accel*dT, 0.0f);

      //This normalises the speed to max speed if travelling diagonally
      GLfloat speed = sqrt(accel.vel.x*accel.vel.x + accel.vel.z*accel.vel.z);
      if (speed > accel.maxSpeed) {
         accel.vel.x *= accel.maxSpeed / speed;
         accel.vel.z *= accel.maxSpeed / speed;
      }

      //Need to normalise facing
      glm::vec3 dirNorm = glm::normalize(facing->dir), rightNorm = glm::normalize(facing->right);
      pos.pos += (accel.vel.z * dirNorm + accel.vel.x * rightNorm) * dT * velMod;

   }

   //If jumping and return to ground, stop jump
   if ((jump) && (pos.pos.y < 3.0f)) {
      jump->isJump = false;
      pos.pos.y = 3.0f;
   }
}





void MoveSystem::moveObject(ex::Entity& ent, Position& pos, Acceleration& accel, GLfloat dT) {

   ex::ComponentHandle<Push> push = ent.component<Push>();
   if (push) {
      if (push->isPush) {
         if (std::abs(push->pushDir.x) > std::abs(push->pushDir.z)) {
            if (push->pushDir.x < 0)
               accel.vel.x = std::min(accel.vel.x + accel.accel*dT, accel.maxSpeed);
            else
               accel.vel.x = std::max(accel.vel.x - accel.accel*dT, -accel.maxSpeed);
         } else {
            if (push->pushDir.z < 0)
               accel.vel.z = std::min(accel.vel.z + accel.accel*dT, accel.maxSpeed);
            else
               accel.vel.z = std::max(accel.vel.z - accel.accel*dT, -accel.maxSpeed);
         }

         push->isPush = false;
      } else {
         accel.vel.x = accel.vel.x < 0 ? std::min(accel.vel.x + accel.accel*dT, 0.0f)
                                       : std::max(accel.vel.x - accel.accel*dT, 0.0f);

         accel.vel.z = accel.vel.z < 0 ? std::min(accel.vel.z + accel.accel*dT, 0.0f)
                                       : std::max(accel.vel.z - accel.accel*dT, 0.0f);
      }
   }

   pos.pos += accel.vel * dT;
}
