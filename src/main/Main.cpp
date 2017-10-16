////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Main.cpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Main.hpp"
//#include <fstream>


int main () {

   GLFWwindow* window;

   //Initialises the graphics and window libraries
   if (initGraphics(window)) {
      std::cerr << "Initialisation failed. Aborting\n";
      return -1;
   }

   //Create the entity systems
   Game* firstGame = new Game(window);

   std::ofstream timeFile; timeFile.open("times.dat");
   //timeFile << "#Frame\tInput\tMove\tBev\tCollision\tMenu\tRender\ttotal" << std::endl;


   //Game loop
   GLint cnt=0; GLfloat cTime = glfwGetTime(); firstGame->frameCount=0;
   GLfloat currT = glfwGetTime();
   do {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear screen
      glfwPollEvents(); //Check for key and mouse events

      //Update every system
      firstGame->update(glfwGetTime() - currT, timeFile);

      currT = glfwGetTime();

      // Checks the frame rate
      if (cnt++ % 500 == 0) {
         std::cout << 500/(currT - cTime) << std::endl;
         cTime = glfwGetTime();
      }

      //Swap second buffer
      glfwSwapBuffers(window);

   } while (glfwWindowShouldClose(window) == 0);

   delete firstGame;
   //Assume destroys window and all associated parts
   glfwTerminate();

   return 0;
}





Game::Game(GLFWwindow* window) {

   ex::Entity entity = entities.create();
   entity.assign<Screen>(0, -1);

   // Level Systems
   systems.add<ObjectSystem>(entities, events);
   systems.add<RoomSystem>(entities, events);
   systems.add<InputSystem>(window, entities);
   systems.add<MoveSystem>(entities);
   systems.add<BevSystem>(entities);
   systems.add<CollisionSystem>();


   // Menu Systems
   systems.add<MenuGenSystem>(); //This doesn't really need a system at the moment
   systems.add<MenuCtrlSystem>(entities);

   //Global Systems
   systems.add<RenderSystem>(entities);
   systems.configure();

   events.emit<GenMenu>(entities, events);
}







void Game::update(ex::TimeDelta dT, std::ofstream& fOut) {

   // GLfloat cTime = glfwGetTime();
   // fOut << frameCount++ << ",\t";

   //systems.update_all(dT);

   //systems.update<ObjectSystem>(dT);
   //systems.update<RoomSystem>(dT);
   systems.update<InputSystem>(dT);

   // fOut << glfwGetTime() - cTime << ",\t";
   // cTime = glfwGetTime();

   systems.update<MoveSystem>(dT);

   // fOut << glfwGetTime() - cTime << ",\t";
   // cTime = glfwGetTime();

   systems.update<BevSystem>(dT);

   // fOut << glfwGetTime() - cTime << ",\t";
   // cTime = glfwGetTime();

   systems.update<CollisionSystem>(dT);

   // fOut << glfwGetTime() - cTime << ",\t";
   // cTime = glfwGetTime();

   //systems.update<MenuGenSystem>(dT);
   systems.update<MenuCtrlSystem>(dT);

   // fOut << glfwGetTime() - cTime << ",\t";
   // //glFinish();
   // cTime = glfwGetTime();

   systems.update<RenderSystem>(dT);

   //glFinish();
   //fOut << glfwGetTime() - cTime << ",\t";
}






int initGraphics(GLFWwindow*& window) {


   if (!glfwInit()) {
      std::cerr << "Failed to initialise GLFW\n";
      return -1;
   }

   //glfwWindowHint(GLFW_SAMPLES, 4); //Antialiasing = 4 samples
   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //Pointing to version of openGL
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Should find out about other profiles

   //Enable mutlisampling Antialiasing
   // glfwWindowHint(GLFW_SAMPLES, 4);
   // glEnable(GL_MULTISAMPLE);

   //Gets information about the primary monitor
   GLFWmonitor* monitor = glfwGetPrimaryMonitor();
   const GLFWvidmode* mode = glfwGetVideoMode(monitor);
   glfwWindowHint(GLFW_RED_BITS, mode->redBits);
   glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
   glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
   glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

   //Creates the window
   //window = glfwCreateWindow(mode->width, mode->height, "First Program", NULL, NULL);

   //window = glfwCreateWindow(1920, 1080, "First Program", monitor, NULL);
   window = glfwCreateWindow(960, 540, "First Program", monitor, NULL);

   //This one sets up full screen but there is currently a bug that produces a blue strip
   //window = glfwCreateWindow(mode->width, mode->height, "First Program", mode, NULL);

   //Checks to ensure a window was created properly
   if (window == NULL) {
       std::cerr << "Failed to open GLFW window\n";
       return -1;
   }
   glfwMakeContextCurrent(window); //Makes this window the current window

   glewExperimental = true; //Find out what this means
   if (glewInit() != GLEW_OK) {
       std::cerr << "Failed to initialise GLEW\n";
       return -1;
   }

   glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); //Allows key presses to be detected in frame
   //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Hide cursor

   glClearColor(0.0, 0.0, 0.4, 0.0);   //Set default backgrond to dark blue
   glEnable(GL_DEPTH_TEST);            //Enable depth testing of objects
   glDepthFunc(GL_LESS);               //Accept if fragment is closer
   glEnable(GL_CULL_FACE);             //Don't draw backfacing triangles#

   glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   return 0;
}
