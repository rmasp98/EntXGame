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
   struct atlas {
      GLuint tex;		         // texture object
      unsigned int w;			// width of texture in pixels
      unsigned int h;			// height of texture in pixels

      struct {
         // advance.x, advance.y, bitmap.width, bitmap.height, bitmap_left, bitmap_top, x and y offset of glyph in texture coordinates
         float ax, ay, bw, bh, bl, bt, tx, ty;
      } c[128];

      atlas(FT_Face, GLuint);
      ~atlas() { glDeleteTextures(1, &tex); };
   };

   public:
      explicit MenuSystem(ex::EntityManager& entM);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   private:
      void genMenu(ex::EntityManager&, GLuint);
      void genFont(ex::EntityManager&, GLuint);
      void makeTextBox(const char*, ex::EntityManager&, glm::vec3, atlas*, GLuint);

      void render_text(const char*, atlas*, float, float, float, float);
};




#endif // MENU_HPP
