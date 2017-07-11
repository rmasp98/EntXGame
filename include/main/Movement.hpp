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
      void configure(ex::EventManager&);
      void receive(const UpdatePos&);

   protected:
      bool isRelease;
      Input* input;

      void moveCharacter(ex::Entity&, Position&, Acceleration&, Direction&, Jump&, GLfloat);
      void moveObject(ex::Entity&, Position&, Acceleration&, Push&, GLfloat);
      void changeDirection(Direction&, GLfloat);
};




#endif // MOVEMENT_HPP
