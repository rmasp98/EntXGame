////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Json.hpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#ifndef JSON_HPP
#define JSON_HPP

// Graphics libraries
#include <glm/glm.hpp>
#include <GL/gl.h>

// Libraries for reading and processing JSON
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
namespace rj = rapidjson;


	std::string getStringKey(rj::Value& mainKey, std::string newKey);
	GLuint getUintKey(rj::Value& mainKey, std::string newKey);
	GLint getIntKey(rj::Value& mainKey, std::string newKey);
	GLfloat getFloatKey(rj::Value& mainKey, std::string newKey);
	bool getBoolKey(rj::Value& mainKey, std::string newKey);
	rj::Value& getArrayKey(rj::Value& mainKey, std::string newKey);
	glm::vec3 getVec3Key(rj::Value& mainKey, std::string newKey);
	bool checkKey(rj::Value& mainKey, std::string newKey, glm::vec3& keyOut);


#endif // JSON_HPP
