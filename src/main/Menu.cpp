////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Menu.cpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Menu.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


const GLuint WIDTH = 800, HEIGHT = 600;
GLuint VAO, VBO;
GLuint w, h;


MenuSystem::MenuSystem(ex::EntityManager& entM) {

	std::string vsName = "shaders/menu.vs", fsName = "shaders/menu.fs";
   if (vsName != "" && fsName != "")
      pID = LoadShaders(vsName.c_str(), fsName.c_str());
   else {
      std::cerr << "Failed to load shaders!\n";
      exit(EXIT_FAILURE);
   }
	glUseProgram(pID);

	if(FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	a42 = new atlas("assets/FreeSans.ttf", 48, ft);

	FT_Done_FreeType(ft);

	renderText(entM, "Hello World", glm::vec2(0.0f));



	 // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}





void MenuSystem::update(ex::EntityManager& entM, ex::EventManager& evtM, ex::TimeDelta dt) {

	RenderText("Hello World", glm::vec2(0.0f, 0.0f), 1.0f/1000, glm::vec3(0.0, 0.0f, 0.0f));

}





void MenuSystem::renderText(ex::EntityManager& entM, std::string text, glm::vec2 tPos) {

	ex::Entity entity = entM.create();

	GLfloat scale = 1.0f/1000.0f;

	GLuint numVerts = 6 * text.length();
	std::vector<glm::vec3> verts(numVerts, glm::vec3(0)), norms(numVerts, glm::vec3(0));
	std::vector<glm::vec2> uvs(numVerts, glm::vec2(0));

	// Iterate through all characters
	const GLubyte *p; GLuint i=0;
	for (p = (const GLubyte *)text.c_str(); *p; p++) {
		 character ch = a42->c[*p];
		 //Character ch = Characters[*c];

		 GLfloat xpos = tPos.x + ch.bearing.x * scale;
		 GLfloat ypos = tPos.y - (ch.size.y - ch.bearing.y) * scale;

		 GLfloat w = ch.size.x * scale;
		 GLfloat h = ch.size.y * scale;
		 // Update VBO for each character
		 GLfloat right = ch.offset.x + (ch.size.x / (GLfloat)a42->w);
		 GLfloat top = ch.offset.y + (ch.size.y / (GLfloat)a42->h);

		 verts[6*i]   = glm::vec3(xpos, ypos + h, 0);
		 uvs  [6*i]   = glm::vec2(ch.offset.x, ch.offset.y);
		 verts[6*i+1] = glm::vec3(xpos, ypos, 0);
		 uvs  [6*i+1] = glm::vec2(ch.offset.x, top);
		 verts[6*i+2] = glm::vec3(xpos + w, ypos, 0);
		 uvs  [6*i+2] = glm::vec2(right, top);

		 verts[6*i+3] = glm::vec3(xpos,     ypos + h, 0);
		 uvs  [6*i+3] = glm::vec2(ch.offset.x, ch.offset.y);
		 verts[6*i+4] = glm::vec3(xpos + w, ypos, 0);
		 uvs  [6*i+4] = glm::vec2(right, top);
		 verts[6*i+5] = glm::vec3(xpos + w, ypos + h, 0);
		 uvs  [6*(i++)+5] = glm::vec2(right, ch.offset.y);

		 tPos.x += (ch.advance.x >> 6) * scale;
   }

	entity.assign<Renderable>(verts, norms, uvs, a42->texID);


}





void MenuSystem::RenderText(std::string text, glm::vec2 pos, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state
    glUseProgram(pID);
    glUniform3f(glGetUniformLocation(pID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    const GLubyte *p; GLuint i=0;
    for (p = (const GLubyte *)text.c_str(); *p; p++) {
		  character ch = a42->c[*p];
		  //Character ch = Characters[*c];

        GLfloat xpos = pos.x + ch.bearing.x * scale;
        GLfloat ypos = pos.y - (ch.size.y - ch.bearing.y) * scale;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;
        // Update VBO for each character
		  GLfloat right = ch.offset.x + (ch.size.x / (GLfloat)a42->w);
		  GLfloat top = ch.offset.y + (ch.size.y / (GLfloat)a42->h);

		  GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   ch.offset.x, ch.offset.y },
            { xpos,     ypos,       ch.offset.x, top },
            { xpos + w, ypos,       right, top },

            { xpos,     ypos + h,   ch.offset.x, ch.offset.y },
            { xpos + w, ypos,       right, top },
            { xpos + w, ypos + h,   right, ch.offset.y }
        };


        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, a42->texID);

	     // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
		  glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        pos.x += (ch.advance.x >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	 }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}







MenuSystem::atlas::atlas(std::string fontFile, GLuint fontSize, FT_Library& ft) {
	FT_Face face;
	if (FT_New_Face(ft, fontFile.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	 // Set size to load glyphs as
	 FT_Set_Pixel_Sizes(face, 0, fontSize);
	 FT_GlyphSlot g = face->glyph;

	 GLuint rowW = 0, rowH = 0;
	 w = 0; h = 0;
	 GLint MAXSIZE;
	 glGetIntegerv(GL_MAX_TEXTURE_SIZE, &MAXSIZE);

	 for (GLubyte i=32; i<128; i++) {
		 if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			 std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
			 continue;
		 }

		 if (rowW + g->bitmap.width + 1 >= (GLuint)MAXSIZE) {
			w = std::max(w, rowW);
			h += rowH;
			rowW = 0;
			rowH = 0;
		}
		rowW += g->bitmap.width + 1;
		rowH = std::max(rowH, g->bitmap.rows);
	}

	w = std::max(w, rowW);
	h += rowH;

	// Generate the texture buffer
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	//Bunch of texture config
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLint xOff=0, yOff=0; rowH=0;
	for (GLubyte i=32; i<128; i++) {
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
			continue;
		}

		if (xOff + g->bitmap.width + 1 >= (GLuint)MAXSIZE) {
			yOff += rowH; rowH = 0; xOff = 0;
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, xOff, yOff, g->bitmap.width,
							 g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		c[i] = character{glm::vec2(g->bitmap.width, g->bitmap.rows),
							  glm::vec2(g->bitmap_left, g->bitmap_top),
						  	  glm::vec2(xOff / (GLfloat)w, yOff / (GLfloat)h),
						  	  glm::ivec2(g->advance.x, g->advance.y)};

		rowH = std::max(rowH, g->bitmap.rows);
		xOff += g->bitmap.width + 1;

	}

	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
}



MenuSystem::atlas::~atlas() {
	glDeleteTextures(1, &texID);
	//FT_Done_Face(face);
}
