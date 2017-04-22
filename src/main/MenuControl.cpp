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

}




void MenuCtrlSystem::update(ex::EntityManager& entM, ex::EventManager& evtM, ex::TimeDelta dT) {

   bool isMenu;
   entM.each<IsMenu>([this, &isMenu](ex::Entity roomEnt, IsMenu& menu) {
      isMenu = menu.isOn;
   });

   if (isMenu) {
      //Finds the current cursor position
      entM.each<Clickable, Font>([this, &entM](ex::Entity entity, Clickable& click, Font& font) {
         double xPos, yPos;
         glfwGetCursorPos(win, &xPos, &yPos);

         if ((xPos > click.bound.x) && (yPos > click.bound.y) && (xPos < click.bound.z) && (yPos < click.bound.w)) {
            font.colour = font.hiColour;

            if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
               entM.each<IsMenu>([this](ex::Entity roomEnt, IsMenu& menu) {
                  menu.isOn = false;
               });
            }
         } else
            font.colour = font.loColour;
      });
   }
}
