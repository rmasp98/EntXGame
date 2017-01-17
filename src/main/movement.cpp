#include <cmath>
#include "main/movement.hpp"










MoveSystem::MoveSystem(GLFWwindow* window) {

   win = window;

}




void MoveSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {

   entM.each<Camera>([this, dT](ex::Entity entity, Camera& cam) {
      changeDirection();
   });

   entM.each<Jump, Acceleration, Position>([this, dT](ex::Entity entity, Position& pos, Acceleration& accel, Jump& jump) {
      jumpObject();
   });

   entM.each<Position, Acceleration>([this, dT](ex::Entity entity, Position& pos, Acceleration& accel) {
      moveObject(pos, accel, dT);
   });

   entM.each<Camera, Position, Acceleration>([this, dT](ex::Entity entity, Camera& cam, Position& pos, Acceleration& accel) {
      cam.view = lookAt(pos.pos, pos.pos + accel.dir, glm::vec3(0,1,0));
   });


}




void MoveSystem::changeDirection() {



}





void MoveSystem::moveObject(Position& pos, Acceleration& accel, float dT) {


   //can make the min/max as a function of yspeed so that diagonal is normalised
   if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
      accel.speed.x = std::min(accel.speed.x + accel.accel*dT,
                               (float)sqrt(accel.maxSpeed*accel.maxSpeed - accel.speed.z*accel.speed.z));
   else if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
      accel.speed.x = std::max(accel.speed.x - accel.accel*dT,
                              (float)-sqrt(accel.maxSpeed*accel.maxSpeed - accel.speed.z*accel.speed.z));
   else
      accel.speed.x = accel.speed.x < 0 ? std::min(accel.speed.x + accel.accel*dT, 0.0f)
                                        : std::max(accel.speed.x - accel.accel*dT, 0.0f);


   if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
      accel.speed.z = std::min(accel.speed.z + accel.accel*dT,
                               (float)sqrt(accel.maxSpeed*accel.maxSpeed - accel.speed.x*accel.speed.x));
   else if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
      accel.speed.z = std::max(accel.speed.z - accel.accel*dT,
                              (float)-sqrt(accel.maxSpeed*accel.maxSpeed - accel.speed.x*accel.speed.x));
   else
      accel.speed.z = accel.speed.z < 0 ? std::min(accel.speed.z + accel.accel*dT, 0.0f)
                                        : std::max(accel.speed.z - accel.accel*dT, 0.0f);

   //if ((!isJump) & (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS)) {
      //isJump = true; isPreJump = true;
   //}


   glm::vec3 mov =   accel.speed.z * accel.dir + accel.speed.x * accel.right;
   tempPos = pos.pos + mov;


   /*//Need to stop changes in movement while jumping
   if (isJump) {
      if (isPreJump) {
         jumpMove = mov; isPreJump = false;
      }

      pos[1] += vz * dT;
      vz += -9.81 * dT;

      tempPos = pos + jumpMove;

      if (pos[1] < 2.0) {
         pos[1] = 2.0;
         isJump = false;
         vz = jumpSpeed;
         isPreJump = true;
      }
   }*/

   //collisionDetect(pos, tempPos);
   pos.pos = tempPos;

}
