#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "main/level.hpp"



int initGraphics(GLFWwindow* window) {


   if (!glfwInit()) {
      std::cerr << "Failed to initialise GLFW\n";
      return -1;
   }

   glfwWindowHint(GLFW_SAMPLES, 4); //Antialiasing = 4 samples
   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Pointing to version of openGL
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Should find out about other profiles

   window = glfwCreateWindow(1024, 768, "First Program", glfwGetPrimaryMonitor(), NULL);
   if (window == NULL) {
       std::cerr << "Failed to open GLFW window\n";
       return -1;
   }
   glfwMakeContextCurrent(window);

   glewExperimental = true; //Find out what this means
   if (glewInit() != GLEW_OK) {
       std::cerr << "Failed to initialise GLEW\n";
       return -1;
   }

   glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE); //Allows key presses to be detected in frame
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Hide cursor
   glfwSetCursorPos(window, 1024/2, 768/2);

   glClearColor(0.0, 0.0, 0.4, 0.0);   //Set default backgrond to dark blue
   glEnable(GL_DEPTH_TEST);            //Enable depth testing of objects
   glDepthFunc(GL_LESS);               //Accept if fragment is closer
   glEnable(GL_CULL_FACE);             //Don't draw backfacing triangles

   return 0;
}





int main () {

   GLFWwindow* window;

   if (initGraphics(window)) {
      std::cerr << "Initialisation failed. Aborting\n";
      return -1;
   }

   // Load, create and initialise object, camara and light
   // objectList* firstRoom = new objectList("shaders/main.vs", "shaders/main.fs");
   //firstRoom->createObject("shaders/roomUV.DDS", glm::mat4(1.0f));
   //firstRoom->createObject("shaders/statue.obj", "shaders/statueUV.DDS", glm::translate(glm::mat4(1.0f), vec3(0.0f, 2.0f, 0.0f)));
   //firstRoom->cam = new camara(window);

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear screen
        glfwPollEvents();

        //Computes new camaraspace position and draw object
        //firstRoom->drawScene(window);

        //Swap buffers
        glfwSwapBuffers(window);

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0);

    //delete firstRoom;
    glfwTerminate();

    return 0;
}
