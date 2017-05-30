////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Movement.cpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Input.hpp"


InputSystem::InputSystem(GLFWwindow* window) {

   //Passes in the window for use throughout this system
   win = window;

   //Determine the center point of window for mouse reset
   glfwGetWindowSize(win, &winXcen, &winYcen);
   winXcen /= 2; winYcen /= 2;

   isRelease = true;

}



void InputSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {

   GLuint currScrn;
   entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
      currScrn = screen.id;
   });

   //This will need to be fixed so that it returns to 10 only when started at 10
   if ((glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) && (isRelease) && (currScrn >= 10)) {
      entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
         if (currScrn >= 20)
            screen.id = screen.prevId;
         else {
            screen.id = 20; screen.prevId = currScrn;
         }
      });
      isRelease = false;
      glfwSetCursorPos(win, winXcen, winYcen);
   } else if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
      isRelease = true;

}
