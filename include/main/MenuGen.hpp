////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Menu.hpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#ifndef MENU_HPP
#define MENU_HPP

//Game headers
#include "main/Components.hpp"
#include "common/objLoader.hpp"
#include "common/shader.hpp"
#include "common/texture.hpp"

// Window Library
#include <GLFW/glfw3.h>


class MenuSystem : public ex::System<MenuSystem> {
   public:
      explicit MenuSystem(ex::EntityManager& entM, GLFWwindow*);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   private:
      GLuint pID;
      GLint scaleX, scaleY;

      void renderText(ex::EntityManager&, std::string, glm::vec3, Atlas&);
};




#endif // MENU_HPP
