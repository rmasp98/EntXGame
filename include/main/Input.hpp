////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                              BirdsEyeView.hpp                                      //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_HPP
#define INPUT_HPP

//Game headers
#include "main/Components.hpp"

//Graphic libraries
#include <GLFW/glfw3.h>


//System for building the puzzle room
class InputSystem : public ex::System<InputSystem> {
   public:
      explicit InputSystem(GLFWwindow*, ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      GLFWwindow* win;
      bool isRelease;
      GLint winXcen, winYcen;

};

#endif // INPUT_HPP
