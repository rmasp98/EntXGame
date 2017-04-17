////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                               MenuControl.hpp                                      //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef MENU_CONTROL_HPP
#define MENU_CONTROL_HPP

//Game headers
#include "main/Components.hpp"

//Graphic libraries
#include <GLFW/glfw3.h>

class MenuCtrlSystem : public ex::System<MenuCtrlSystem> {
   public:
      explicit MenuCtrlSystem(GLFWwindow*);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      GLFWwindow* win;

};





#endif // MENU_CONTROL_HPP
