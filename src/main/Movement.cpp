#include <cmath>
#include "main/Movement.hpp"




MoveSystem::MoveSystem(GLFWwindow* window) {

   win = window;

   glfwGetWindowSize(win, &winXcen, &winYcen);
   winXcen /= 2; winYcen /= 2;

   glfwSetCursorPos(win, winXcen, winYcen);

}




void MoveSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {

   entM.each<Camera, Direction>([this, dT](ex::Entity entity, Camera& cam, Direction& face) {
      changeDirection(cam, face, dT);
   });

   entM.each<Position, Acceleration, Direction>([this, dT](ex::Entity entity, Position& pos,
                                                           Acceleration& accel, Direction& face) {
      moveObject(entity, pos, accel, face, dT);
   });

   entM.each<Camera, Position, Direction>([this](ex::Entity entity, Camera& cam, Position& pos, Direction& face) {
      cam.view = lookAt(pos.pos, pos.pos + face.facing, glm::vec3(0,1,0));
   });


}




void MoveSystem::changeDirection(Camera& cam, Direction& dir, GLfloat dT) {

   double xPos, yPos;
   glfwGetCursorPos(win, &xPos, &yPos);

   dir.angle.x += dir.mouseSpeed * dT * GLfloat(winXcen - xPos);
   double dTheta = dir.mouseSpeed * dT * GLfloat(winYcen - yPos);
   if (dir.angle.y + dTheta <= -0.9 * glm::pi<GLfloat>() / 2.0f)
      dir.angle.y = -0.9 * glm::pi<GLfloat>() / 2.0f;
   else if (dir.angle.y + dTheta > 0.9 * glm::pi<GLfloat>() / 2.0f)
      dir.angle.y = 0.9 * glm::pi<GLfloat>() / 2.0f;
   else
      dir.angle.y += dTheta;

   dir.facing = glm::vec3(cos(dir.angle.y) * sin(dir.angle.x),
                          sin(dir.angle.y),
                          cos(dir.angle.y) * cos(dir.angle.x));

   dir.dir = glm::vec3(dir.facing.x, 0, dir.facing.z);
   dir.right = glm::vec3(dir.facing.z, 0, -dir.facing.x);

   glfwSetCursorPos(win, winXcen, winYcen);

}





void MoveSystem::moveObject(ex::Entity& ent, Position& pos, Acceleration& accel, Direction& facing, GLfloat dT) {

   bool isJump = false;
   ex::ComponentHandle<Jump> jump = ent.component<Jump>();
   if (jump) {
      if ((glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS) && (!jump->isJump)) {
         jump->isJump = true; accel.vel.y = jump->jumpSpeed;
      }

      isJump = jump->isJump;
   }


   glm::vec3 tempPos(pos.pos);
   if (isJump) {

      tempPos.y += accel.vel.y * dT;
      accel.vel.y += jump->gravity * dT;

   } else {
      //can make the min/max as a function of yspeed so that diagonal is normalised
      if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
         accel.vel.x = std::min(accel.vel.x + accel.accel*dT, accel.maxSpeed);
      else if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
         accel.vel.x = std::max(accel.vel.x - accel.accel*dT, -accel.maxSpeed);
      else
         accel.vel.x = accel.vel.x < 0 ? std::min(accel.vel.x + accel.accel*dT, 0.0f)
                                           : std::max(accel.vel.x - accel.accel*dT, 0.0f);


      if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
         accel.vel.z = std::min(accel.vel.z + accel.accel*dT, accel.maxSpeed);
      else if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
         accel.vel.z = std::max(accel.vel.z - accel.accel*dT, -accel.maxSpeed);
      else
         accel.vel.z = accel.vel.z < 0 ? std::min(accel.vel.z + accel.accel*dT, 0.0f)
                                           : std::max(accel.vel.z - accel.accel*dT, 0.0f);

      GLfloat speed = sqrt(accel.vel.x*accel.vel.x + accel.vel.z*accel.vel.z);
      if (speed > accel.maxSpeed) {
         accel.vel.x *= accel.maxSpeed / speed;
         accel.vel.z *= accel.maxSpeed / speed;
      }
   }

   tempPos += accel.vel.z * facing.dir + accel.vel.x * facing.right;

   //collision detection

   pos.pos = tempPos;

   if ((jump) && (pos.pos.y < 0)) {
      jump->isJump = false;
      pos.pos.y = 0;
   }


}
