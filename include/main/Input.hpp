////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                 Input.hpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_HPP
#define INPUT_HPP

//Game headers
#include "main/Components.hpp"
#include "main/Json.hpp"

//Graphic libraries
#include <GLFW/glfw3.h>

// String library for file ingest
#include <fstream>
#include <sstream>


//System for building the puzzle room
class InputSystem : public ex::System<InputSystem> {
   public:
      explicit InputSystem(GLFWwindow*, ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      GLFWwindow* win;
      GLuint oldKeyState, keyState, holdKeys;
      std::vector<GLdouble> winCen;
      std::map<std::string, GLint> keys;
      std::map<std::string, GLuint> keyMap;

      void assignKeys(std::string);
};

#endif // INPUT_HPP
