////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Movement.cpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Input.hpp"


InputSystem::InputSystem(GLFWwindow* window, ex::EntityManager& entM) {

   //Passes in the window for use throughout this system
   win = window;

   //Determine the center point of window for mouse reset
   GLint tempCen[2];
   glfwGetWindowSize(win, &tempCen[0], &tempCen[1]);

   winCen.resize(2,0);
   winCen[0] = (GLdouble)tempCen[0] / 2.0; winCen[1] = (GLdouble)tempCen[1] / 2.0;

   ex::Entity entity = entM.create();
   entity.assign<Input>(winCen);

   oldKeyState = 0;
   assignKeys();

}



void InputSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {

   GLuint currScrn;
   entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
      currScrn = screen.id;
   });

   keyState = 1;
   for (GLuint i=0; i < keys.size(); i++) {
      if (glfwGetKey(win, keys[i]) == GLFW_PRESS)
         keyState += glm::pow(2,i);
   }

   // Keys that have just been pressed = (keys that have changed) & keys that are pressed
   GLuint activeKeys = (keyState ^ oldKeyState) & keyState;
   oldKeyState = keyState;

   //Finds the current cursor position
   std::vector<GLdouble> cursor(2, 0);
   glfwGetCursorPos(win, &cursor[0], &cursor[1]);

   if (currScrn == 10)
      glfwSetCursorPos(win, winCen[0], winCen[1]);

   entM.each<Input>([this, &activeKeys, &cursor](ex::Entity roomEnt, Input& input) {
      input.active = activeKeys;
      input.cursor = cursor;
   });



   //This will need to be fixed so that it returns to 10 only when started at 10
   if ((activeKeys & 1) && (currScrn >= 10)) {
      entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
         if (currScrn >= 20)
            screen.id = screen.prevId;
         else {
            screen.id = 20; screen.prevId = currScrn;
         }
      });

      glfwSetCursorPos(win, winCen[0], winCen[1]);
   }
}






void InputSystem::assignKeys() {
   //This will later go in a function that will read from file.
   keys.resize(10,0);
   keys[0] = 256; //Escape
   keys[1] = 340; //Left_SHIFT
   keys[2] = 32;  //Space
   keys[3] = 87;  //W
   keys[4] = 65;  //A
   keys[5] = 83;  //S
   keys[6] = 68;  //D
   keys[7] = 77;  //M
}
