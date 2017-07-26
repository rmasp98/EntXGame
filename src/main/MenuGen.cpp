////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                MenuGen.cpp                                         //
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

		// Get font values from config and load font
		std::string fontFile = getStringKey(doc, "fontFile");
		GLuint fontBaseSize = getUintKey(doc, "fontBaseSize");
		Atlas* menuFont = new Atlas(fontFile, fontBaseSize, ft);

		// Read in the main font colours
		glm::vec3 mainBC = getVec3Key(doc, "baseColour");
		glm::vec3 mainHC = getVec3Key(doc, "highColour");

		// Check for the menu config and loop over each menu
		rj::Value& menu = getArrayKey(doc, "menus");
		for (rj::SizeType iMenu = 0; iMenu < menu.Size(); ++iMenu) {
			GLuint menuID = getUintKey(menu[iMenu], "id");

			// If there are menu specific font colours, override main colours
			glm::vec3 menuBC;
			if (!checkKey(menu[iMenu], "baseColour", menuBC))
				menuBC = mainBC;

			glm::vec3 menuHC;
			if (!checkKey(menu[iMenu], "highColour", menuHC))
				menuHC = mainHC;

			std::string bgImage = "";
			if (menu[iMenu].HasMember("bgImage"))
				bgImage = getStringKey(menu[iMenu], "bgImage");

			genBackground(entM, bgImage, menuID);

			// Loop over all buttons in the menu

			rj::Value& buttons = getArrayKey(menu[iMenu], "buttons");
			for (rj::SizeType jButton = 0; jButton < buttons.Size(); ++jButton) {
				/*if (buttons[jButton].HasMember("autoGen") && getBoolKey(buttons[jButton], "autoGen")) {
					std::map<std::string, GLuint[2]> keyMap;
				   entM.each<Input>([this, &keyMap](ex::Entity roomEnt, Input& input) {
				      keyMap = input.keyMap;
				   });

					GLfloat fontSize = 1.0;
					if (menu[iMenu].HasMember("fontSize"))
						fontSize = getFloatKey(menu[iMenu], "fontSize");

					GLuint it=0;
					for (auto const &ent1 : keyMap) {
						for (GLuint iKey=0; iKey<2; iKey++) {
							ex::Entity entity = entM.create();

							//Need tp set pos
							glm::vec3 pos((iKey - 0.5) * 0.5, 0.2 - it * 0.15, 0.0f);
							makeButton(entity, ent1.first, pos, 0, fontSize, *menuFont);

							entity.assign<Shader>(pID);
							entity.assign<MenuID>(1, menuID);

							glm::vec3 buttonBC;
							if (!checkKey(menu[iMenu], "baseColour", buttonBC))
								buttonBC = menuBC;

							glm::vec3 buttonHC;
							if (!checkKey(menu[iMenu], "highColour", buttonHC))
								buttonHC = menuHC;

							entity.assign<Font>(buttonBC, buttonHC, menuFont);
						} it++;
					}
				} else {*/


				std::vector<std::string> text;
				std::vector<glm::vec3> pos;
				std::vector<GLuint> action;
				if (buttons[jButton].HasMember("autoGen") && getBoolKey(buttons[jButton], "autoGen")) {
					std::map<std::string, GLuint[2]> keyMap;
				   entM.each<Input>([this, &keyMap](ex::Entity roomEnt, Input& input) {
				      keyMap = input.keyMap;
				   });

					//glfwGetKeyName(keycode, 0);

					glm::vec3 offset = getVec3Key(buttons[jButton], "position");
					GLfloat space = getFloatKey(buttons[jButton], "space");

					GLuint it=0;
					for (auto const &ent1 : keyMap) {
						//key action name
						text.push_back(ent1.first);
						pos.push_back(offset + glm::vec3(space * -1, it * (-0.1), 0.0f)); //calculate this
						action.push_back(0);

						//Key action assignment
						text.push_back("hello");
						pos.push_back(offset + glm::vec3(space, it++ * (-0.1), 0.0f));
						action.push_back(3);
					}
				} else {
					text.push_back(getStringKey(buttons[jButton], "text"));
					pos.push_back(getVec3Key(buttons[jButton], "position"));
					action.push_back(getUintKey(buttons[jButton], "actionId"));
				}

				GLfloat fontSize = 1.0;
				if (buttons[jButton].HasMember("fontSize"))
					fontSize = getFloatKey(buttons[jButton], "fontSize");

				for (GLuint kButtons=0; kButtons<text.size(); kButtons++) {
					ex::Entity entity = entM.create();

					makeButton(entity, text[kButtons], pos[kButtons], action[kButtons],
								  fontSize, *menuFont);

					entity.assign<Shader>(pID);
					entity.assign<MenuID>(0, menuID);

					//Probably should have a check that if actionId==1 then there is a linkId
					if (buttons[jButton].HasMember("linkId"))
						entity.assign<ScreenLink>(getIntKey(buttons[jButton], "linkId"));

					glm::vec3 buttonBC;
					if (!checkKey(buttons[jButton], "baseColour", buttonBC))
						buttonBC = menuBC;

					glm::vec3 buttonHC;
					if (!checkKey(buttons[jButton], "highColour", buttonHC))
						buttonHC = menuHC;

					entity.assign<Font>(buttonBC, buttonHC, menuFont);

					// Quick bodge to set the default button colour
					ex::ComponentHandle<Renderable> mesh = entity.component<Renderable>();
					if (mesh)
						mesh->colour = buttonBC;
				}
			}
		}
	}

	FT_Done_FreeType(ft);
}





void MenuGenSystem::genBackground(ex::EntityManager& entM, std::string bgImage, GLuint menuID) {
	ex::Entity entity = entM.create();

	//Doesn't work at the moment but I would like to get it working
	//GLuint texID = SOIL_load_OGL_texture(bgImage.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
	//		SOIL_FLAG_INVERT_Y);

	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);*/

	//Load texture for object
	GLint texID = 0;
	if (bgImage != "") {
   	texID = loadDDS(bgImage.c_str());

		if (texID == 0)
			std::cout << "Failed to load menu image" << std::endl;
	}


	std::vector<glm::vec3> verts(6, glm::vec3(0.0f)), norms(6, glm::vec3(0.0f));
	std::vector<glm::vec2> uvs(6, glm::vec2(0.0f));

	verts[0] = glm::vec3(-1.0f, 1.0f, 0.1f);
	verts[1] = glm::vec3(-1.0f, -1.0f, 0.1f);
	verts[2] = glm::vec3(1.0f, -1.0f, 0.1f);

	uvs[0] = glm::vec2(0.0f, 1.0f);
	uvs[1] = glm::vec2(0.0f, 0.0f);
	uvs[2] = glm::vec2(1.0f, 0.0f);

	verts[3] = glm::vec3(-1.0f, 1.0f, 0.1f);
	verts[4] = glm::vec3(1.0f, -1.0f, 0.1f);
	verts[5] = glm::vec3(1.0f, 1.0f, 0.1f);

	uvs[3] = glm::vec2(0.0f, 1.0f);
	uvs[4] = glm::vec2(1.0f, 0.0f);
	uvs[5] = glm::vec2(1.0f, 1.0f);

	//Currently still passing the text texture to shader
	entity.assign<Renderable>(verts, norms, uvs, texID);
	entity.assign<Shader>(pID);
	entity.assign<MenuID>(0, menuID);
}




void MenuGenSystem::makeButton(ex::Entity& entity, std::string text, glm::vec3 pos, GLuint action, GLfloat fSize, Atlas& font) {

	GLuint numVerts = 6 * text.length();
	std::vector<glm::vec3> verts(numVerts, glm::vec3(0)), norms(numVerts, glm::vec3(0));
	std::vector<glm::vec2> uvs(numVerts, glm::vec2(0));

	// Iterate through all characters
	GLfloat xOff = 0, yOff = 0, maxH=0; GLuint i=0;
	const GLubyte *p;
	for (p = (const GLubyte *)text.c_str(); *p; p++) {
		 character ch = font.c[*p];

		 GLfloat xpos = xOff + ch.bearing.x / (GLfloat)scaleX * fSize;
		 GLfloat ypos = yOff - (ch.size.y - ch.bearing.y) / (GLfloat)scaleY * fSize;

		 GLfloat w = ch.size.x / (GLfloat)scaleX * fSize;
		 GLfloat h = ch.size.y / (GLfloat)scaleY * fSize;

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

		 xOff += (ch.advance.x >> 6) / (GLfloat)scaleX * fSize;
		 yOff += (ch.advance.y >> 6) / (GLfloat)scaleY * fSize;

		 maxH = std::max(maxH, h);
   }

	entity.assign<Renderable>(verts, norms, uvs, font.texID);

	pos.x -= xOff / 2.0f;
	entity.assign<Position>(pos, glm::vec3(0.0f));
	entity.assign<Action>(action);

	// Cursor Y Pos is inverted hence the order and inversion of the y coords
	// Also everything is in texture coord, which is tranformed to pixel coords
	if (action) {
		entity.assign<Clickable>((pos.x + 1) * scaleX / 2.0f,
										 (1 - (pos.y + maxH)) * scaleY / 2.0f,
										 (pos.x + xOff + 1) * scaleX / 2.0f,
										 (1 - pos.y) * scaleY / 2.0f);
	}

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
