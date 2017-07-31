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
      entM.each<Clickable, Font, Action, MenuID, Renderable>([this, &entM, &evtM, &currScrn]
               (ex::Entity entity, Clickable& click, Font& font, Action& action, MenuID& menu, Renderable& mesh) {

         double xPos, yPos;
         glfwGetCursorPos(win, &xPos, &yPos);

         // If the cursor is on top of the button
         if ((xPos > click.bound.x) && (yPos > click.bound.y) && (xPos < click.bound.z) && (yPos < click.bound.w)) {
            mesh.colour = font.hiColour;

            // If the button is clicked
            if ((menu.id == currScrn) && (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS))
               buttonPress(action.functionID, entM, evtM, entity);

         } else
            mesh.colour = font.loColour;
      });
   }
}






void MenuCtrlSystem::buttonPress(GLuint functionID, ex::EntityManager& entM, ex::EventManager& evtM, ex::Entity& entity) {

   if (functionID == 1) {
      ex::ComponentHandle<ScreenLink> link = entity.component<ScreenLink>();
      if (link) {
         entM.each<Screen>([this, &link](ex::Entity roomEnt, Screen& screen) {
            if (link->linkId >= 0)
               screen.id = link->linkId;
            else if (screen.prevId >= 0)
               screen.id = screen.prevId;

            glfwSetCursorPos(win, winXcen/2.0f, winYcen/2.0f);
         });
      }
   } else if (functionID == 2)
      glfwSetWindowShouldClose(win, GLFW_TRUE);
   else if (functionID == 3) {
      evtM.emit<GenRoom>(entM, evtM);
   } else if (functionID == 4)
      evtM.emit<GenObjects>();
}
