////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Movement.cpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Movement.hpp"



MoveSystem::MoveSystem(GLFWwindow* window) {

   //Passes in the window for use throughout this system
   win = window;

   //Determine the center point of window for mouse reset
   glfwGetWindowSize(win, &winXcen, &winYcen);
   winXcen /= 2; winYcen /= 2;

   //Move cursor to the center
   glfwSetCursorPos(win, winXcen, winYcen);

}




void MoveSystem::configure(ex::EventManager &evnM) { evnM.subscribe<PushEvent>(*this); }





void MoveSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {

   //Look at mouse movement to determine if facing direction changed
   entM.each<Camera, Direction>([this, dT](ex::Entity entity, Camera& cam, Direction& face) {
      changeDirection(cam, face, dT);
   });

   //Checks for each controllable entity that moves and moves them (currently only moves camera)
   entM.each<Position, Acceleration>([this, dT](ex::Entity entity, Position& pos, Acceleration& accel) {

      ex::ComponentHandle<Direction> face = entity.component<Direction>();

      if (face)
         moveObject(entity, pos, accel, face.get(), dT);
      else
         moveObject(pos, accel, dT);

   });

   //Creates the model matrix based on the objects current position
   entM.each<Position, Renderable>([this](ex::Entity entity, Position& pos, Renderable& mat) {
      mat.modelMat = glm::translate(glm::mat4(1.0f), pos.pos);
   });

   //Assigns the calculated values to the camera
   entM.each<Camera, Position, Direction>([this](ex::Entity entity, Camera& cam, Position& pos, Direction& face) {
      cam.view = lookAt(pos.pos, pos.pos + face.facing, glm::vec3(0,1,0));
   });


}




void MoveSystem::changeDirection(Camera& cam, Direction& dir, GLfloat dT) {

   //Finds the current cursor position
   double xPos, yPos;
   glfwGetCursorPos(win, &xPos, &yPos);

   //Determines the angle of the camera based on how far the cursor position has changed
   dir.angle.x += dir.mouseSpeed * dT * GLfloat(winXcen - xPos);
   double dTheta = dir.mouseSpeed * dT * GLfloat(winYcen - yPos);

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

   //Reset the cursor position
   glfwSetCursorPos(win, winXcen, winYcen);

}





void MoveSystem::moveObject(ex::Entity& ent, Position& pos, Acceleration& accel, Direction* facing, GLfloat dT) {

   //Some entities don't have jump so need to account for that
   bool isJump = false;
   ex::ComponentHandle<Jump> jump = ent.component<Jump>();
   if (jump) {
      //If spacebar is pressed initiate jump
      if ((glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS) && (!jump->isJump)) {
         jump->isJump = true; accel.vel.y = jump->jumpSpeed;
      }

      isJump = jump->isJump;
   }

   //If jump is true, keypresses have no affect
   if (isJump) {
      //Updates the y position and velocity and the updates xz position
      pos.pos.y += accel.vel.y * dT;
      accel.vel.y += jump->gravity * dT;
      pos.pos += accel.vel.z * facing->dir + accel.vel.x * facing->right;
   } else {
      //If A/D is pressed, increase x speed up to max speed otherwise decelerate down to 0
      if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
         accel.vel.x = std::min(accel.vel.x + accel.accel*dT, accel.maxSpeed);
      else if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
         accel.vel.x = std::max(accel.vel.x - accel.accel*dT, -accel.maxSpeed);
      else
         //This ensures that it stops at zero and doesn't accelerate in the opposite direction
         accel.vel.x = accel.vel.x < 0 ? std::min(accel.vel.x + accel.accel*dT, 0.0f)
                                           : std::max(accel.vel.x - accel.accel*dT, 0.0f);

      //Same as above but for z direction
      if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
         accel.vel.z = std::min(accel.vel.z + accel.accel*dT, accel.maxSpeed);
      else if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
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

      //This then updates position and adds a sprint if shift is pressed
      if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
         pos.pos += (accel.vel.z * facing->dir + accel.vel.x * facing->right) * 2.0f;
      else
         pos.pos += accel.vel.z * facing->dir + accel.vel.x * facing->right;

   }

   //If jumping and return to ground, stop jump
   if ((jump) && (pos.pos.y < 2.0f)) {
      jump->isJump = false;
      pos.pos.y = 2.0f;
   }
}





void MoveSystem::moveObject(Position& pos, Acceleration& accel, GLfloat dT) {



}




void MoveSystem::receive(const PushEvent& push) {

   GLfloat dT = 0.5f;

   ex::Entity ent = push.object;

   ex::ComponentHandle<Acceleration> objMove = ent.component<Acceleration>();
   ex::ComponentHandle<Position> objPos = ent.component<Position>();
   if (objMove && objPos) {
      if (std::abs(push.distVec.x) > std::abs(push.distVec.z)) {
         if (push.distVec.x < 0)
            objMove->vel.x = std::min(objMove->vel.x + objMove->accel*dT, objMove->maxSpeed);
         else
            objMove->vel.x = std::max(objMove->vel.x - objMove->accel*dT, -objMove->maxSpeed);
      } else {
         if (push.distVec.z < 0)
            objMove->vel.z = std::min(objMove->vel.z + objMove->accel*dT, objMove->maxSpeed);
         else
            objMove->vel.z = std::max(objMove->vel.z - objMove->accel*dT, -objMove->maxSpeed);
      }
   }
}
