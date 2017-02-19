////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Menu.hpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#ifndef MENU_HPP
#define MENU_HPP

//Graphics libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <entityx/entityx.h>
namespace ex = entityx;

//Game headers
#include "common/objLoader.hpp"
#include "common/shader.hpp"
#include "common/texture.hpp"
#include "main/Components.hpp"


class MenuSystem : public ex::System<MenuSystem> {
   public:
      explicit MenuSystem(ex::EntityManager& entM);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   private:
      void genMenu(ex::EntityManager&);
};




#endif // MENU_HPP
