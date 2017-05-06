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

// String library for file ingest
#include <fstream>
#include <sstream>

// Libraries for reading and processing JSON
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
namespace rj = rapidjson;


class MenuGenSystem : public ex::System<MenuGenSystem> {
   public:
      explicit MenuGenSystem(ex::EntityManager& entM, GLFWwindow*);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   private:
      GLuint pID;
      GLint scaleX, scaleY;

      void makeButton(ex::EntityManager&, std::string, glm::vec3, Atlas&);
      void readConfig(ex::EntityManager&, std::string);

      void getKey(rj::Value&, std::string, std::string&);
      void getKey(rj::Value&, std::string, GLuint&);
      void getKey(rj::Value&, std::string, GLint&);
      void getKey(rj::Value&, std::string, GLfloat&);
      rj::Value& getKey(rj::Value&, std::string);

};




#endif // MENU_HPP
