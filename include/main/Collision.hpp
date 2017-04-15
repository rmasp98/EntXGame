////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Collision.hpp                                       //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_HPP
#define COLLISION_HPP

//Game headers
#include "main/Components.hpp"


class CollisionSystem : public ex::System<CollisionSystem> {
   public:
      explicit CollisionSystem();
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      void wallCollision(Position&, Room&);
      void objectCollision(ex::Entity&, ex::Entity&, ex::EventManager&);
};


#endif // COLLISION_HPP
