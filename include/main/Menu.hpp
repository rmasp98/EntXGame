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

//Text libraries
#include <ft2build.h>
#include FT_FREETYPE_H

//Game headers
#include "common/objLoader.hpp"
#include "common/shader.hpp"
#include "common/texture.hpp"
#include "main/Components.hpp"

#define MAXWIDTH 1024


class MenuSystem : public ex::System<MenuSystem> {
   public:
      explicit MenuSystem(ex::EntityManager& entM);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   private:
      struct character {
         glm::vec2 size, bearing, offset;
         glm::ivec2 advance;
      };

      struct atlas {
         GLuint texID;		      // texture object
         GLuint w, h;			   // width and height of texture in pixels
         character c[128];

         atlas(std::string, GLuint, FT_Library& ft);
         ~atlas();
      };
      FT_Library ft;
      GLuint pID;
      atlas* a42;

      void renderText(ex::EntityManager&, std::string, glm::vec2);//, atlas&);
      void RenderText(std::string, glm::vec2, GLfloat, glm::vec3);
      //void genMenu(ex::EntityManager&, GLuint);
      //void genFont(ex::EntityManager&, GLuint);
      //void makeTextBox(const char*, ex::EntityManager&, glm::vec3, atlas*, GLuint);

      //void render_text(const char*, atlas*, float, float, float, float);
};




#endif // MENU_HPP
