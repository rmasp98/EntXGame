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

	// Read the json file into rapidjson
	std::ifstream cfgFile; cfgFile.open(fileName);
	rj::IStreamWrapper cfgIn(cfgFile);

	// If json file has been parsed successfully
	rj::Document doc;
	if (!doc.ParseStream(cfgIn).HasParseError()) {
		// Get the menu shaders from config and load them
		std::string vsName = getStringKey(doc, "vertShader");
		std::string fsName = getStringKey(doc, "fragShader");
		if (vsName != "" && fsName != "")
	      pID = LoadShaders(vsName.c_str(), fsName.c_str());
	   else {
	      std::cerr << "Failed to load shaders!\n";
	      exit(EXIT_FAILURE);
	   }

		getKey<GLuint>(doc, "fontSize");

		// Get font values from config and load font
		std::string fontFile = getStringKey(doc, "fontFile");
		GLuint fontSize = getUintKey(doc, "fontSize");
		Atlas* menuFont = new Atlas(fontFile, fontSize, ft);

		// Read in the main font colours
		glm::vec3 mainBC = getVec3Key(doc, "baseColour");
		glm::vec3 mainHC = getVec3Key(doc, "highColour");

		// Check for the menu config and loop over each menu
		rj::Value& menu = getArrayKey(doc, "menus");
		for (rj::SizeType i = 0; i < menu.Size(); ++i) {
			GLuint menuID = getUintKey(menu[i], "id");

			// If there are menu specific font colours, override main colours
			glm::vec3 menuBC;
			if (!checkKey(menu[i], "baseColour", menuBC))
				menuBC = mainBC;

			glm::vec3 menuHC;
			if (!checkKey(menu[i], "highColour", menuHC))
				menuHC = mainHC;

			// Loop over all buttons in the menu
			rj::Value& buttons = getArrayKey(menu[i], "buttons");
			for (rj::SizeType j = 0; j < buttons.Size(); ++j) {
				GLuint buttonID = getUintKey(buttons[j], "id");
				std::string buttonText = getStringKey(buttons[j], "text");
				glm::vec3 buttonPos = getVec3Key(buttons[j], "position");

				ex::Entity entity = entM.create();
				makeButton(entity, buttonText, buttonPos, *menuFont);

				entity.assign<Shader>(pID);
				entity.assign<MenuID>(menuID);

				glm::vec3 buttonBC;
				if (!checkKey(buttons[j], "baseColour", buttonBC))
					buttonBC = menuBC;

				glm::vec3 buttonHC;
				if (!checkKey(buttons[j], "highColour", buttonHC))
					buttonHC = menuHC;

				entity.assign<Font>(buttonBC, buttonHC, menuFont);
			}
		}
	}

	FT_Done_FreeType(ft);
}



//This doesn't work :( When you pass a type most returns fail
/*template<typename T>
T MenuGenSystem::getKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];

	if (typeid(T) == typeid(std::string)) {
		assert(keyRef.IsString()); return keyRef.GetString();
	} else if (typeid(T) == typeid(GLuint)) {
		assert(keyRef.IsUint()); return keyRef.GetUint();
	} else if (typeid(T) == typeid(GLint)) {
		assert(keyRef.IsInt()); return keyRef.GetInt();
	} else if (typeid(T) ==  typeid(GLfloat)) {
		assert(keyRef.IsDouble()); return keyRef.GetDouble();
	} else if (typeid(T) ==  typeid(rj::Value&)) {
		assert(keyRef.IsArray()); return keyRef;
	} else if (typeid(T) ==  typeid(glm::vec3)) {
		assert(keyRef.IsArray());
		glm::vec3 keyOut;
		for (GLuint k = 0; k < 3; ++k)
		  keyOut[k] = keyRef[k].GetDouble();

		return keyOut;
	} else
		return NULL;
}*/


std::string MenuGenSystem::getStringKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsString()); return keyRef.GetString();
}

GLuint MenuGenSystem::getUintKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsUint()); return keyRef.GetUint();
}

GLint MenuGenSystem::getIntKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsInt()); return keyRef.GetInt();
}

GLfloat MenuGenSystem::getFloatKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsDouble()); return keyRef.GetDouble();
}

rj::Value& MenuGenSystem::getArrayKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsArray()); return keyRef;
}

glm::vec3 MenuGenSystem::getVec3Key(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsArray());

	glm::vec3 keyOut;
	for (GLuint k = 0; k < 3; ++k)
		keyOut[k] = keyRef[k].GetDouble();

	return keyOut;
}


bool MenuGenSystem::checkKey(rj::Value& mainKey, std::string newKey, glm::vec3& keyOut) {
	if (mainKey.HasMember(newKey.c_str()) && (mainKey[newKey.c_str()].IsArray())) {
		for (GLuint k = 0; k < 3; ++k)
			keyOut[k] = mainKey[newKey.c_str()][k].GetDouble();

		return true;
	} else
		return false;
}







void MenuGenSystem::makeButton(ex::Entity& entity, std::string text, glm::vec3 pos, Atlas& font) {

	GLuint numVerts = 6 * text.length();
	std::vector<glm::vec3> verts(numVerts, glm::vec3(0)), norms(numVerts, glm::vec3(0));
	std::vector<glm::vec2> uvs(numVerts, glm::vec2(0));

	// Iterate through all characters
	GLfloat xOff = 0, yOff = 0, maxH=0; GLuint i=0;
	const GLubyte *p;
	for (p = (const GLubyte *)text.c_str(); *p; p++) {
		 character ch = font.c[*p];

		 GLfloat xpos = xOff + ch.bearing.x / (GLfloat)scaleX;
		 GLfloat ypos = yOff - (ch.size.y - ch.bearing.y) / (GLfloat)scaleY;

		 GLfloat w = ch.size.x / (GLfloat)scaleX;
		 GLfloat h = ch.size.y / (GLfloat)scaleY;

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

	entity.assign<Renderable>(verts, norms, uvs, font.texID);

	pos.x -= xOff / 2.0f;
	entity.assign<Position>(pos, 0.0f);

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
