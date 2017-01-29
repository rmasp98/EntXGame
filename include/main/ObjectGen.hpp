////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                ObjectGen.cpp                                       //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_GEN_HPP
#define OBJECT_GEN_HPP

//Graphics libraries
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <entityx/entityx.h>
namespace ex = entityx;

//Game headers
#include "common/objLoader.hpp"
#include "common/shader.hpp"
#include "common/texture.hpp"
#include "main/Components.hpp"


class ObjectSystem : public ex::System<ObjectSystem> {
   public:
      explicit ObjectSystem(ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      ex::EntityManager* entMan;

      void genLevel();
      void genObject(std::string, std::string, GLint);
};


#endif // OBJECT_GEN_HPP
