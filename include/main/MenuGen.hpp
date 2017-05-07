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
      struct button {
         button(glm::vec3 bcIn, glm::vec3 hcIn) : bc(bcIn), hc(hcIn) {};
         glm::vec3 bc, hc;
      };

      GLuint pID;
      GLint scaleX, scaleY;

      void makeButton(ex::Entity&, std::string, glm::vec3, bool, GLfloat, Atlas&);
      void readConfig(ex::EntityManager&, std::string);

      std::string getStringKey(rj::Value&, std::string);
      GLuint getUintKey(rj::Value&, std::string);
      GLint getIntKey(rj::Value&, std::string);
      GLfloat getFloatKey(rj::Value&, std::string);
      bool getBoolKey(rj::Value&, std::string);
      glm::vec3 getVec3Key(rj::Value&, std::string);

      rj::Value& getArrayKey(rj::Value&, std::string);
      bool checkKey(rj::Value&, std::string, glm::vec3&);

};




#endif // MENU_HPP
