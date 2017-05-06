////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Menu.cpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/MenuGen.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



MenuGenSystem::MenuGenSystem(ex::EntityManager& entM, GLFWwindow* window) {

	//Get window size for text scaling
	glfwGetWindowSize(window, &scaleX, &scaleY);

	// Read in the config file and generates the menus from there
	readConfig(entM, "config/menu.cfg");
}





void MenuGenSystem::update(ex::EntityManager& entM, ex::EventManager& evtM, ex::TimeDelta dT) {


}




void MenuGenSystem::readConfig(ex::EntityManager& entM, std::string fileName) {

	FT_Library ft;
	if(FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	std::string line;

	std::ifstream cfgFile; cfgFile.open(fileName);
	rj::IStreamWrapper cfgIn(cfgFile);

	rj::Document doc;
	if (!doc.ParseStream(cfgIn).HasParseError()) {
		// Get the menu shaders from config and load them
		std::string vsName, fsName;
		getKey(doc, "vertShader", vsName); getKey(doc, "fragShader", fsName);
		if (vsName != "" && fsName != "")
	      pID = LoadShaders(vsName.c_str(), fsName.c_str());
	   else {
	      std::cerr << "Failed to load shaders!\n";
	      exit(EXIT_FAILURE);
	   }

		// Get font values from config and load font
		std::string fontFile; GLuint fontSize;
		getKey(doc, "fontFile", fontFile); getKey(doc, "fontSize", fontSize);
		Atlas* menuFont = new Atlas(fontFile, fontSize, ft);

		rj::Value& menu = getKey(doc, "menus");
		for (rj::SizeType i = 0; i < menu.Size(); ++i) {
			GLuint menuID = menu[i]["id"].GetInt();

			rj::Value& buttons = getKey(menu[i], "buttons");
			for (rj::SizeType j = 0; j < buttons.Size(); ++j) {
				GLuint buttonID; std::string buttonText;
				getKey(buttons[j], "id", buttonID);
				getKey(buttons[j], "text", buttonText);

				glm::vec3 buttonPos;
				rj::Value& posKey = getKey(buttons[j], "position");
				for (GLuint k = 0; k < 3; ++k)
					buttonPos[k] = posKey[k].GetDouble();

				makeButton(entM, buttonText, buttonPos, *menuFont);
			}
		}
	}

	FT_Done_FreeType(ft);
}



/*// Keytype = 0 (object), 1 (string), 2 (int), 3 (uint), 4 (double), 5 (array)
template<typename T>
void MenuGenSystem::getKey(rj::Value& mainKey, std::string newKey, T& keyOut) {
	assert(mainKey.HasMember(newKey))
	rj::Value& keyRef = mainKey[newKey];

	switch (keyRef.GetType()) {
		case 0: assert(!keyRef.IsObject());
		case 1: assert(!keyRef.IsString());
		case 2: assert(!keyRef.IsInt());
		case 3: assert(!keyRef.IsUint());
		case 4: assert(!keyRef.IsDouble());
		case 5: assert(!keyRef.IsObject());
	}
}*/



void MenuGenSystem::getKey(rj::Value& mainKey, std::string newKey, std::string& keyOut) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsString()); keyOut = keyRef.GetString();
}

void MenuGenSystem::getKey(rj::Value& mainKey, std::string newKey, GLuint& keyOut) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsUint()); keyOut = keyRef.GetUint();
}

void MenuGenSystem::getKey(rj::Value& mainKey, std::string newKey, GLint& keyOut) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsInt()); keyOut = keyRef.GetInt();
}

void MenuGenSystem::getKey(rj::Value& mainKey, std::string newKey, GLfloat& keyOut) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsDouble()); keyOut = keyRef.GetDouble();
}

rj::Value& MenuGenSystem::getKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsArray()); return keyRef;
}







void MenuGenSystem::makeButton(ex::EntityManager& entM, std::string text, glm::vec3 pos, Atlas& font) {

	ex::Entity entity = entM.create();

	GLuint numVerts = 6 * text.length();
	std::vector<glm::vec3> verts(numVerts, glm::vec3(0)), norms(numVerts, glm::vec3(0));
	std::vector<glm::vec2> uvs(numVerts, glm::vec2(0));

	// Iterate through all characters
	GLfloat xOff = 0, yOff = 0, maxH=0; GLuint i=0;
	const GLubyte *p;
	for (p = (const GLubyte *)text.c_str(); *p; p++) {
		 character ch = font.c[*p];
		 //Character ch = Characters[*c];

		 GLfloat xpos = xOff + ch.bearing.x / (GLfloat)scaleX;
		 GLfloat ypos = yOff - (ch.size.y - ch.bearing.y) / (GLfloat)scaleY;

		 GLfloat w = ch.size.x / (GLfloat)scaleX;
		 GLfloat h = ch.size.y / (GLfloat)scaleY;
		 // Update VBO for each character
		 GLfloat right = ch.offset.x + (ch.size.x / (GLfloat)font.w);
		 GLfloat top = ch.offset.y + (ch.size.y / (GLfloat)font.h);

		 verts[6*i]   = glm::vec3(xpos, ypos + h, 0);
		 verts[6*i+1] = glm::vec3(xpos, ypos, 0);
		 verts[6*i+2] = glm::vec3(xpos + w, ypos, 0);
		 uvs  [6*i]   = glm::vec2(ch.offset.x, ch.offset.y);
		 uvs  [6*i+1] = glm::vec2(ch.offset.x, top);
		 uvs  [6*i+2] = glm::vec2(right, top);

		 verts[6*i+3] = glm::vec3(xpos,     ypos + h, 0);
		 verts[6*i+4] = glm::vec3(xpos + w, ypos, 0);
		 verts[6*i+5] = glm::vec3(xpos + w, ypos + h, 0);
		 uvs  [6*i+3] = glm::vec2(ch.offset.x, ch.offset.y);
		 uvs  [6*i+4] = glm::vec2(right, top);
		 uvs  [6*(i++)+5] = glm::vec2(right, ch.offset.y);

		 xOff += (ch.advance.x >> 6) / (GLfloat)scaleX;
		 yOff += (ch.advance.y >> 6) / (GLfloat)scaleY;

		 maxH = std::max(maxH, h);
   }

	entity.assign<MenuID>(0);
	entity.assign<Renderable>(verts, norms, uvs, font.texID);
	entity.assign<Shader>(pID);

	pos.x -= xOff / 2.0f;
	entity.assign<Position>(pos, 0.0f);
	entity.assign<Font>(glm::vec3(0.9f, 0.3f, 0.3f),
	 						  glm::vec3(1.0f, 1.0f, 1.0f),
							  &font);

	// Cursor Y Pos is inverted hence the order and inversion of the y coords
	// Also everything is in texture coord, which is tranformed to pixel coords
	entity.assign<Clickable>((pos.x + 1) * scaleX / 2.0f,
									 (1 - (pos.y + maxH)) * scaleY / 2.0f,
									 (pos.x + xOff + 1) * scaleX / 2.0f,
									 (1 - pos.y) * scaleY / 2.0f,
									 0);

}





Atlas::Atlas(std::string fontFile, GLuint fontSize, FT_Library& ft) {
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
	std::vector<GLubyte> emptyData(w * h * 4, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, &emptyData[0]);


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
