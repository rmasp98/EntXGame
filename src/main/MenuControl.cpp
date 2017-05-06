////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                               MenuControl.cpp                                      //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#include "main/MenuControl.hpp"



MenuCtrlSystem::MenuCtrlSystem(GLFWwindow* window) {

   win = window;
   glfwGetWindowSize(win, &winXcen, &winYcen);

}




void MenuCtrlSystem::update(ex::EntityManager& entM, ex::EventManager& evtM, ex::TimeDelta dT) {

   GLuint currScrn;
   entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
      currScrn = screen.id;
   });

   if (currScrn == 0) {
      //Finds the current cursor position
      entM.each<Clickable, Font>([this, &entM](ex::Entity entity, Clickable& click, Font& font) {
         double xPos, yPos;
         glfwGetCursorPos(win, &xPos, &yPos);



         // If the cursor is on top of the button
         if ((xPos > click.bound.x) && (yPos > click.bound.y) && (xPos < click.bound.z) && (yPos < click.bound.w)) {
            font.colour = font.hiColour;

            // If the button is clicked
            if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
               buttonPress(click.buttonID, entM);

         } else
            font.colour = font.loColour;
      });
   }
}






void MenuCtrlSystem::buttonPress(GLuint buttonType, ex::EntityManager& entM) {

   switch(buttonType) {
      case 0 : entM.each<Screen>([this](ex::Entity roomEnt, Screen& screen) {
                  screen.id = 1;

                  glfwSetCursorPos(win, winXcen/2.0f, winYcen/2.0f);
               });
               break;
      //case 1 : std::cout << "Not done this yet" << std::endl;
               //break;
   }
}
