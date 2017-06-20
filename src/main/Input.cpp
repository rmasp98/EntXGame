////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                 Input.cpp                                          //
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

   assignKeys("config/keys.cfg");

   ex::Entity entity = entM.create();
   entity.assign<Input>(winCen, keyMap);

   oldKeyState = 0;

}




void InputSystem::update(ex::EntityManager& entM, ex::EventManager& evnM, ex::TimeDelta dT) {

   GLuint currScrn;
   entM.each<Screen>([this, &currScrn](ex::Entity roomEnt, Screen& screen) {
      currScrn = screen.id;
   });

   keyState = 0;
   GLuint it=0;
   for (auto const &ent1 : keyMap) {
      if (glfwGetKey(win, ent1.second[0]) == GLFW_PRESS)
         keyState += glm::pow(2,it);
      it++;
   }

   // key state changed if keyState xor (not the same as) oldKeyState
   GLuint stateChange = keyState ^ oldKeyState;
   // active keys if the key is a hold key or has changed and is pressed down
   GLuint activeKeys = (holdKeys | stateChange) & keyState;
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
   if ((activeKeys & keyMap["menu"][1]) && (currScrn >= 10)) {
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






void InputSystem::assignKeys(std::string fileName) {

   std::map<std::string, bool> holdKeyTemp;
   holdKeys = 0;

   // Read the json file into rapidjson
	std::ifstream cfgFile; cfgFile.open(fileName);
	rj::IStreamWrapper cfgIn(cfgFile);

   // If json file has been parsed successfully
	rj::Document doc;
	if (!doc.ParseStream(cfgIn).HasParseError()) {
      // Check for the menu config and loop over each menu
		rj::Value& type = getArrayKey(doc, "inputType");
		for (rj::SizeType iType = 0; iType < type.Size(); ++iType) {
         if (getStringKey(type[iType], "type") == "keyboard") {
            rj::Value& keysIn = getArrayKey(type[iType], "keys");
            for (rj::SizeType jKey = 0; jKey < keysIn.Size(); ++jKey) {
               std::string keyName = getStringKey(keysIn[jKey], "name");
               keyMap[keyName][0] = getUintKey(keysIn[jKey], "keyVal");

               // Find out if key is a hold key
               holdKeyTemp[keyName] = false;
               if (keysIn[jKey].HasMember("holdKey")) {
                  if (getBoolKey(keysIn[jKey], "holdKey"))
                     holdKeyTemp[keyName] = true;
               }
            }
         }
      }
   }

   // Finish set up of keys, setting hold keys and key mapping
   GLuint it=0;
   for (auto const &ent1 : keyMap) {
      // If it is a hold key add it to binary chain
      if (holdKeyTemp[ent1.first])
         holdKeys += pow(2, it);

      // Place where key is in the binary chain based on map order
      keyMap[ent1.first][1] = glm::pow(2, it++);
   }
}
