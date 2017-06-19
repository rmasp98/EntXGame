////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Json.cpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Json.hpp"


std::string getStringKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsString()); return keyRef.GetString();
}

GLuint getUintKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsUint()); return keyRef.GetUint();
}

GLint getIntKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsInt()); return keyRef.GetInt();
}

GLfloat getFloatKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsDouble()); return keyRef.GetDouble();
}

bool getBoolKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsBool()); return keyRef.GetBool();
}

rj::Value& getArrayKey(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsArray()); return keyRef;
}

glm::vec3 getVec3Key(rj::Value& mainKey, std::string newKey) {
	assert(mainKey.HasMember(newKey.c_str())); rj::Value& keyRef = mainKey[newKey.c_str()];
	assert(keyRef.IsArray());

	glm::vec3 keyOut;
	for (GLuint k = 0; k < 3; ++k)
		keyOut[k] = keyRef[k].GetDouble();

	return keyOut;
}


bool checkKey(rj::Value& mainKey, std::string newKey, glm::vec3& keyOut) {
	if (mainKey.HasMember(newKey.c_str()) && (mainKey[newKey.c_str()].IsArray())) {
		for (GLuint k = 0; k < 3; ++k)
			keyOut[k] = mainKey[newKey.c_str()][k].GetDouble();

		return true;
	} else
		return false;
}
