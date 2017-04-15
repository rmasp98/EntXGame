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


class MenuSystem : public ex::System<MenuSystem> {
   public:
      explicit MenuSystem(ex::EntityManager& entM);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   private:
      GLuint pID;

      void renderText(ex::EntityManager&, std::string, Atlas&);
};




#endif // MENU_HPP
