////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                ObjectGen.cpp                                       //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_GEN_HPP
#define OBJECT_GEN_HPP

//Game headers
#include "main/Components.hpp"
#include "common/objLoader.hpp"
#include "common/shader.hpp"
#include "common/texture.hpp"

//Graphics libraries
#include <SOIL/SOIL.h>


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
