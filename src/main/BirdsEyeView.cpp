////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                              BirdsEyeView.cpp                                      //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#include "main/BirdsEyeView.hpp"


BevSystem::BevSystem(GLFWwindow* window) {
   //Passes in the window for use throughout this system
   win = window;
   delay = 0;
}



void BevSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {

   GLuint currScrn;
   entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
      currScrn = screen.id;
   });

   if (currScrn == 11) {
      delay++;
      moveUp(entM);
   } else if (currScrn == 12) {
      moveBev();

      if (glfwGetKey(win, GLFW_KEY_M) == GLFW_PRESS) {
         entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
            screen.id = 13;
         });
         oldBevPos = bevPos; oldViewPos = viewPos;
      }
   } else if (currScrn == 13) {
      delay++;
      moveDown(entM);
   }
}



void BevSystem::moveUp(ex::EntityManager& entM) {

   GLfloat t = 100.0f;
   entM.each<Camera, Position, Direction>([this, &t](ex::Entity entity, Camera& cam, Position& pos, Direction& face) {
      currView = pos.pos + face.facing;
      viewPos = currView;
      GLfloat yT = (1 - cos(glm::pi<GLfloat>()*delay/t))/2.0f;
      bevPos = glm::vec3(pos.pos.x,
                         pos.pos.y + (20.0f - pos.pos.y)*yT,
                         pos.pos.z);

      viewOrient = yT * glm::vec3(1,0,0) + (1 - yT) * glm::vec3(0,1,0);

      cam.view = lookAt(bevPos, viewPos, viewOrient);
   });

   if (delay == t) {
      entM.each<Screen>([this](ex::Entity roomEnt, Screen& screen) {
         screen.id = 12;
      });
      delay = 0;
   }

}



void BevSystem::moveDown(ex::EntityManager& entM) {

   GLfloat t = 100.0f;
   entM.each<Camera, Position, Direction>([this, &t](ex::Entity entity, Camera& cam, Position& pos, Direction& face) {
      GLfloat yT = (1 - cos(glm::pi<GLfloat>()*delay/t))/2.0f;
      bevPos = yT * pos.pos + (1 - yT) * oldBevPos;
      viewOrient = (1 - yT) * glm::vec3(1,0,0) + yT * glm::vec3(0,1,0);
      viewPos = yT * currView + (1 - yT) * oldViewPos;

      cam.view = lookAt(bevPos, viewPos, viewOrient);
   });

   if (delay == t) {
      entM.each<Screen>([this](ex::Entity roomEnt, Screen& screen) {
         screen.id = 10;
      });
      delay = 0;
   }

}



void BevSystem::moveBev() {

   GLfloat moveSpd = 0.1;

   if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
      bevPos += glm::vec3(0.0f, 0.0f, -1.0f) * moveSpd;
      viewPos += glm::vec3(0.0f, 0.0f, -1.0f) * moveSpd;
   } else if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
      bevPos += glm::vec3(0.0f, 0.0f, 1.0f) * moveSpd;
      viewPos += glm::vec3(0.0f, 0.0f, 1.0f) * moveSpd;
   }

   if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
      bevPos += glm::vec3(1.0f, 0.0f, 0.0f) * moveSpd;
      viewPos += glm::vec3(1.0f, 0.0f, 0.0f) * moveSpd;
   } else if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
      bevPos += glm::vec3(-1.0f, 0.0f, 0.0f) * moveSpd;
      viewPos += glm::vec3(-1.0f, 0.0f, 0.0f) * moveSpd;
   }
}
