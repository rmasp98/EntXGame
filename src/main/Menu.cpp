////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Menu.cpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Menu.hpp"


MenuSystem::MenuSystem(ex::EntityManager& entM) {

   //Load the shaders (need to add shader names to config file)
   std::string vsName = "shaders/menu.vs", fsName = "shaders/menu.fs";
   GLuint pID;
   if (vsName != "" && fsName != "")
      pID = LoadShaders(vsName.c_str(), fsName.c_str());
   else {
      std::cerr << "Failed to load shaders!\n";
      exit(EXIT_FAILURE);
   }

   genFont(entM, pID);

   //Make the menu entities
   genMenu(entM, pID);

}





void MenuSystem::update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) {



}







void MenuSystem::genFont(ex::EntityManager& entM, GLuint progID) {

   //Initialise the font library
   FT_Library ft;
   if(FT_Init_FreeType(&ft)) {
      fprintf(stderr, "Could not init freetype library\n");
   }

   //Initialise font
   FT_Face face;
   if(FT_New_Face(ft, "/usr/share/fonts/TTF/LiberationSans-Regular.ttf", 0, &face)) {
      fprintf(stderr, "Could not open font\n");
   }

   atlas* a42 = new atlas(face, 42);

   glm::vec3 tPos = glm::vec3(0.0f);
   makeTextBox("Hello World", entM, tPos, a42, progID);


}





void MenuSystem::makeTextBox(const char* text, ex::EntityManager& eM, glm::vec3 pos, atlas* font, GLuint prog) {

   ex::Entity entity = eM.create();

   const GLubyte *p;
   GLfloat sx=1.0f, sy=1.0f, x=0.0f, y=0.0f;
   std::vector<glm::vec3> verts, norms;
   std::vector<glm::vec2> uvs;
   for (p = (const GLubyte *)text; *p; p++) {
      std::cout << *p << std::endl;
		//Calculate the vertex and texture coordinates
		float x2 = x + font->c[*p].bl * sx;
		float y2 = -y - font->c[*p].bt * sy;
		float w = font->c[*p].bw * sx;
		float h = font->c[*p].bh * sy;

      //Advance the cursor to the start of the next character
		x += font->c[*p].ax * sx;
		y += font->c[*p].ay * sy;

      // Skip glyphs that have no pixels
		if (!w || !h)
			continue;

      verts.push_back(glm::vec3(x2, -y2, 0));
      uvs.push_back(glm::vec2(font->c[*p].tx, font->c[*p].ty));
      norms.push_back(glm::vec3(0.0f));

      verts.push_back(glm::vec3(x2 + w, -y2, 0));
      uvs.push_back(glm::vec2(font->c[*p].tx + font->c[*p].bw / font->w, font->c[*p].ty));
      norms.push_back(glm::vec3(0.0f));

      verts.push_back(glm::vec3(x2, -y2 - h, 0));
      uvs.push_back(glm::vec2(font->c[*p].tx, font->c[*p].ty + font->c[*p].bh / font->h));
      norms.push_back(glm::vec3(0.0f));

      verts.push_back(glm::vec3(x2 + w, -y2, 0));
      uvs.push_back(glm::vec2(font->c[*p].tx + font->c[*p].bw / font->w, font->c[*p].ty));
      norms.push_back(glm::vec3(0.0f));

      verts.push_back(glm::vec3(x2, -y2 - h, 0));
      uvs.push_back(glm::vec2(font->c[*p].tx, font->c[*p].ty + font->c[*p].bh / font->h));
      norms.push_back(glm::vec3(0.0f));

      verts.push_back(glm::vec3(x2 + w, -y2 - h, 0));
      uvs.push_back(glm::vec2(font->c[*p].tx + font->c[*p].bw / font->w, font->c[*p].ty + font->c[*p].bh / font->h));
      norms.push_back(glm::vec3(0.0f));
	}

   //Assign verts, norms and uvs
   entity.assign<Renderable>(verts, norms, uvs, font->tex);
   entity.assign<Shader>(prog);
   entity.assign<Position>(pos, 0.0f);


}






void MenuSystem::genMenu(ex::EntityManager& entM, GLuint progID) {

   ex::Entity entity = entM.create();

   std::vector<glm::vec3> verts, norms;
   std::vector<glm::vec2> uvs;

   verts.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
   verts.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
   verts.push_back(glm::vec3(1.0f, 1.0f, 0.0f));

   norms.push_back(glm::vec3(0,0,-1));
   norms.push_back(glm::vec3(0,0,-1));
   norms.push_back(glm::vec3(0,0,-1));

   uvs.push_back(glm::vec2(0));
   uvs.push_back(glm::vec2(0));
   uvs.push_back(glm::vec2(0));



   verts.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
   verts.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
   verts.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));

   norms.push_back(glm::vec3(0,0,-1));
   norms.push_back(glm::vec3(0,0,-1));
   norms.push_back(glm::vec3(0,0,-1));

   uvs.push_back(glm::vec2(0));
   uvs.push_back(glm::vec2(0));
   uvs.push_back(glm::vec2(0));

   //Assign all values to the entity
   GLuint texID;
   entity.assign<Renderable>(verts, norms, uvs, texID);
   entity.assign<Shader>(progID);
   entity.assign<Position>(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f);

}











MenuSystem::atlas::atlas(FT_Face face, GLuint height) {
   FT_Set_Pixel_Sizes(face, 0, height);
   FT_GlyphSlot g = face->glyph;
   unsigned int roww = 0;
   unsigned int rowh = 0;
   w = 0;
   h = 0;
   memset(c, 0, sizeof c);

   //Find minimum size for a texture holding all visible ASCII characters
   for (int i = 32; i < 128; i++) {
      if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
         fprintf(stderr, "Loading character %c failed!\n", i);
         continue;
      }

      if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
         w = std::max(w, roww); h += rowh;
         roww = 0; rowh = 0;
      }

      roww += g->bitmap.width + 1;
      rowh = std::max(rowh, g->bitmap.rows);
   }

   w = std::max(w, roww);
   h += rowh;

   //Create a texture that will be used to hold all ASCII glyphs
   glActiveTexture(GL_TEXTURE0); glGenTextures(1, &tex);
   glBindTexture(GL_TEXTURE_2D, tex); //glUniform1i(uniform_tex, 0);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

   //We require 1 byte alignment when uploading texture data
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   //Clamping to edges is important to prevent artifacts when scaling
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //Linear filtering usually looks best for text
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   //Paste all glyph bitmaps into the texture, remembering the offset
   int ox = 0, oy = 0; rowh = 0;
   for (int i = 32; i < 128; i++) {
      if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
         fprintf(stderr, "Loading character %c failed!\n", i);
         continue;
      }

      if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
         oy += rowh;
         rowh = 0;
         ox = 0;
      }

      glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
      c[i].ax = g->advance.x >> 6;  c[i].ay = g->advance.y >> 6;
      c[i].bw = g->bitmap.width;    c[i].bh = g->bitmap.rows;
      c[i].bl = g->bitmap_left;     c[i].bt = g->bitmap_top;
      c[i].tx = ox / (float)w;      c[i].ty = oy / (float)h;
      rowh = std::max(rowh, g->bitmap.rows);
      ox += g->bitmap.width + 1;
   }
   fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", w, h, w * h / 1024);
}
