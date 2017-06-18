////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                              BirdsEyeView.hpp                                      //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#ifndef BIRDS_EYE_VIEW_HPP
#define BIRDS_EYE_VIEW_HPP

//Game headers
#include "main/Components.hpp"

//Graphic libraries
#include <GLFW/glfw3.h>


//System for building the puzzle room
class BevSystem : public ex::System<BevSystem> {
   public:
      explicit BevSystem(ex::EntityManager&);
      void update(ex::EntityManager&, ex::EventManager&, ex::TimeDelta) override;

   protected:
      glm::vec3 bevPos, currView, viewOrient, oldBevPos, viewPos, oldViewPos;
      GLuint delay;
      GLfloat moveSpd, tMax;
      bool isBodge;
      Input* input;

      void moveUp(ex::EntityManager&);
      void moveDown(ex::EntityManager&);
      void moveBev();
      void zoomBev();

};

#endif // BIRDS_EYE_VIEW_HPP
