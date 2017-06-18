////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Movement.hpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

//Game headers
#include "main/Components.hpp"

//Generic libraries
#include <cmath>


class MoveSystem : public ex::System<MoveSystem>, public ex::Receiver<MoveSystem> {
   public:
      explicit MoveSystem(ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      bool isRelease;
      Input* input;

      void moveObject(ex::Entity&, Position&, Acceleration&, Direction*, GLfloat);
      void moveObject(ex::Entity&, Position&, Acceleration&, GLfloat);
      void changeDirection(Direction&, GLfloat);
};




#endif // MOVEMENT_HPP
