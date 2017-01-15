#ifndef OBJECT_GEN_HPP
#define OBJECT_GEN_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <entityx/entityx.h>
namespace ex = entityx;

#include "common/objLoader.hpp"
#include "common/shader.hpp"
#include "common/texture.hpp"
#include "main/components.hpp"


class objGenSystem : public ex::System<objGenSystem> {
   public:
      explicit objGenSystem(ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;


};





#endif // OBJECT_GEN_HPP
