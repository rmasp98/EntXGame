////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                MenuGen.hpp                                         //
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
#include "main/Json.hpp"

// Window Library
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

// String library for file ingest
#include <fstream>
#include <sstream>



class MenuGenSystem : public ex::System<MenuGenSystem>, public ex::Receiver<MenuGenSystem> {
   public:
      explicit MenuGenSystem(ex::EntityManager&, ex::EventManager&, GLFWwindow*);
      void configure(ex::EventManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;
      void receive(const GenMenu&);

   private:
      struct button {
         button(glm::vec3 bcIn, glm::vec3 hcIn) : bc(bcIn), hc(hcIn) {};
         glm::vec3 bc, hc;
      };

      GLuint pID;
      GLint scaleX, scaleY;

      void makeButton(ex::Entity&, std::string, glm::vec3, GLuint, GLfloat, Atlas&, ex::EventManager&);
      void genBackground(ex::EntityManager&, ex::EventManager&, std::string, GLuint);
      void readConfig(ex::EntityManager&, ex::EventManager&, std::string);

};




#endif // MENU_HPP
