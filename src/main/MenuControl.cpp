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

   if (currScrn != 10) {
      //Finds the current cursor position
      entM.each<Clickable, Font, Action>([this, &entM]
               (ex::Entity entity, Clickable& click, Font& font, Action& action) {

         double xPos, yPos;
         glfwGetCursorPos(win, &xPos, &yPos);

         // If the cursor is on top of the button
         if ((xPos > click.bound.x) && (yPos > click.bound.y) && (xPos < click.bound.z) && (yPos < click.bound.w)) {
            font.colour = font.hiColour;

            // If the button is clicked
            if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
               buttonPress(action.functionID, entM, entity);

         } else
            font.colour = font.loColour;
      });
   }
}






void MenuCtrlSystem::buttonPress(GLuint functionID, ex::EntityManager& entM, ex::Entity& entity) {

   if (functionID == 0) {
      ex::ComponentHandle<ScreenLink> link = entity.component<ScreenLink>();
      if (link) {
         entM.each<Screen>([this, &link](ex::Entity roomEnt, Screen& screen) {
            screen.id = link->linkId;
            glfwSetCursorPos(win, winXcen/2.0f, winYcen/2.0f);
         });
      }
   } else if (functionID == 1)
      glfwSetWindowShouldClose(win, GLFW_TRUE);;
}
