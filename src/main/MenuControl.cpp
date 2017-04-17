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

   //Finds the current cursor position
   double xPos, yPos;
   glfwGetCursorPos(win, &xPos, &yPos);

   entM.each<Clickable, Font>([this, &xPos, &yPos](ex::Entity entity, Clickable& click, Font& font) {
      if ((xPos > click.bound.x) && (yPos > click.bound.y) && (xPos < click.bound.z) && (yPos < click.bound.w)) {
         font.colour = glm::vec3(1.0f);

         if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            font.colour = glm::vec3(1.0f, 0.0f, 0.0f);
      } else
         font.colour = glm::vec3(0.0f);
   });

}
