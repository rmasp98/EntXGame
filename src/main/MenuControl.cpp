////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                               MenuControl.cpp                                      //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#include "main/MenuControl.hpp"



MenuCtrlSystem::MenuCtrlSystem(ex::EntityManager& entM) {
   entM.each<Input>([this](ex::Entity null, Input& tempInput) {
      input = &tempInput;
   });
}




void MenuCtrlSystem::update(ex::EntityManager& entM, ex::EventManager& evtM, ex::TimeDelta dT) {

   GLuint currScrn;
   entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
      currScrn = screen.id;
   });

   if (currScrn != 10) {
      entM.each<Clickable, Font, Action, MenuID, Material>([this, &entM, &evtM, &currScrn]
               (ex::Entity entity, Clickable& click, Font& font, Action& action, MenuID& menu, Material& mat) {

         // If the cursor is on top of the button
         if ((input->cursor[0] > click.bound.x) && (input->cursor[1] > click.bound.y)
          && (input->cursor[0] < click.bound.z) && (input->cursor[1] < click.bound.w)) {
            mat.colour = font.hiColour;

            // If the button is clicked
            if ((menu.id == currScrn) && (input->active & input->keyMap["select"][1])) {
               buttonPress(action.functionID, entM, evtM, entity);
               currScrn = 10;
            }

         } else
            mat.colour = font.loColour;
      });
   }
}






void MenuCtrlSystem::buttonPress(GLuint functionID, ex::EntityManager& entM, ex::EventManager& evtM, ex::Entity& entity) {

   if (functionID == 1) {
      ex::ComponentHandle<ScreenLink> link = entity.component<ScreenLink>();
      if (link) {
         entM.each<Screen>([this, &link, &evtM](ex::Entity roomEnt, Screen& screen) {
            if (link->linkId >= 0)
               screen.id = link->linkId;
            else if (screen.prevId >= 0)
               screen.id = screen.prevId;

            // This should be an event to the input system so that this system doesn't need window
            if (screen.id == 10)
               evtM.emit<CenterCursor>();

         });
      }
   } else if (functionID == 2)
      evtM.emit<QuitGame>();
   else if (functionID == 3)
      evtM.emit<GenRoom>(entM, evtM);
   else if (functionID == 4)
      evtM.emit<GenObjects>();
}
