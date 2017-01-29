////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                  Main.cpp                                          //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Main.hpp"


int main () {

   GLFWwindow* window;

   //Initialises the graphics and window libraries
   if (initGraphics(window)) {
      std::cerr << "Initialisation failed. Aborting\n";
      return -1;
   }

   //Create the entity system
   Level* firstLevel = new Level(window);
   GLfloat currT = glfwGetTime();

   //Game loop
   do {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear screen
      glfwPollEvents(); //Check for key and mouse events

      //Update every system
      firstLevel->update(glfwGetTime() - currT);
      currT = glfwGetTime();

      //Swap second buffer
      glfwSwapBuffers(window);

   } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0);

   //Assume destroys window and all associated parts
   glfwTerminate();

   return 0;
}







int initGraphics(GLFWwindow*& window) {


   if (!glfwInit()) {
      std::cerr << "Failed to initialise GLFW\n";
      return -1;
   }

   glfwWindowHint(GLFW_SAMPLES, 4); //Antialiasing = 4 samples
   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Pointing to version of openGL
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Should find out about other profiles

   //Gets information about the primary monitor
   const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   glfwWindowHint(GLFW_RED_BITS, mode->redBits);
   glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
   glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
   glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

   //Creates the window
   window = glfwCreateWindow(mode->width, mode->height, "First Program", NULL, NULL);

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
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Hide cursor

   glClearColor(0.0, 0.0, 0.4, 0.0);   //Set default backgrond to dark blue
   glEnable(GL_DEPTH_TEST);            //Enable depth testing of objects
   glDepthFunc(GL_LESS);               //Accept if fragment is closer
   glEnable(GL_CULL_FACE);             //Don't draw backfacing triangles

   return 0;
}
