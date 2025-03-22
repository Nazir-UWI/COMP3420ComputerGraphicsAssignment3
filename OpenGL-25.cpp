//
//  OpenGL-25.cpp
//  OpenGL-25
//
//  Created by Dr. John Charlery on 03/09/2025.
//  Copyright (c) 2025 University of the West Indies. All rights reserved.
//

// ==============================================================================
//  PROGRAM:
//          Callback functions: Animated Textured Sphere - Spinning Earth code,
//          borrowed from OpenGL-17 with Callback functionalities.
//
//          Right-click  - Acknowledges the right button is clicked
//          Left-Click   - Repositions the planet
//          Scroll button- Pans the camera up and down    (With 3-buttons mouse)
//          Left Arrow   - Moves the planet left
//          Right Arrows - Moves the planet right
//          'H'          - Moves the camera "home"
//           ESC         - Terminates the program
// ==========================================================================


// ==============================================================================
//  Steps to create the MVP:
//    1.  Setup and compile our shaders as an object of the Shader class.
//    2.  Load the geometrical model objects.
//    3.  Create the projection matrix.
//    4.  Create the view matrix
//    5.  Create the model matrix – initially as an identity matrix and
//        then modify with scaling, translation, rotation, etc, if required.
//    6.  Pass the model, view and projection matrices to the shaders.
//    7.  Draw the object.
//
// ==============================================================================

/*===============================================================================
Install the following in Package Manager for Windows :

Install-Package glew_dynamic
Install-Package glfw
Install-Package GLMathematics
Install-Package freeimage -version 3.16.0
Install-Package nupengl.core
Install-Package Soil
Install-Package Assimp -version 3.0.0

=================================================================================*/


// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

    // GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL.h>


// GL includes
#include "shader.h"
#include "camera.h"
#include "model.h"

//--------------------------------------------------------------------------
// Callback Functions Prototype
//--------------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow*, int, int, int, int);



// Active window
GLFWwindow* window;

// Properties
GLuint sWidth = 800, sHeight = 600;

//--------------------------------------------------------------------------
// Set up the view port
//--------------------------------------------------------------------------
GLfloat xMin  = -1*(sWidth/2.0f);
GLfloat xMax  = sWidth/2.0f;
GLfloat yMax  = sHeight/2.0f;
GLfloat yMin  = -1*(sHeight/2.0f);
GLfloat zMin  = 1.0f;
GLfloat zMax  = 10000.0f;


// Camera
// Variabes for the x,y,z location of the camera.
GLfloat camX = 0.0f, camY = 200.0f, camZ = 1700.0f;
Camera camera(glm::vec3(camX, camY, camZ));



// Variabes for the x,y,z location of the planet.
GLfloat planetX = 0.0f, planetY = 0.0f, planetZ = 0.0f;



Model planet();
GLchar* modelName = (GLchar*)"planet.obj";


GLfloat lastX  =  sWidth  / 2.0;
GLfloat lastY  =  sHeight / 2.0;
bool    keys[1024];



GLfloat planetAngle = 0.0;


void init_Resources()
    {
        // Init GLFW
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        
        // Define the window
        window = glfwCreateWindow(sWidth, sHeight, "COMP3420 - Step 24 - CallBack Functions", 0, 0);
       
        glfwMakeContextCurrent(window);
        
       
    
        //====================================================================
        //    Set the required callback functions
        //====================================================================
        glfwSetCursorPosCallback(window, mouse_callback);              // The mouse movement
        glfwSetScrollCallback(window, scroll_callback);                // Scrolling camera
        glfwSetMouseButtonCallback(window, mouseButtonCallback);       // The mouse click
        glfwSetKeyCallback(window, key_callback);                      // The keyboard


        // OPTIONAL:  Disable the mouse pointer from being visible
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        //====================================================================
    
    
        // Initialize GLEW to setup the OpenGL Function pointers
        glewExperimental = GL_TRUE;
        glewInit();
    
    
        // Define the viewport dimensions
        glViewport(0, 0, 2*sWidth, 2*sHeight);
    
        // Setup OpenGL options
        glEnable(GL_DEPTH_TEST);
    
    }



// The MAIN function, from here we start our application and run the loop
int main()
{
    init_Resources();
    
    
    
    // ==============================================
    // ====== Set up the stuff for our sphere =======
    // ==============================================
    
    
    // =======================================================================
    // Step 1. Setup and compile our shaders as an object of the Shader class
    // =======================================================================
    Shader planetShader("vertex25.glsl", "fragment25.glsl");

    
    
    
    
    // =======================================================================
    // Step 2. Load the geometrical model objects
    // =======================================================================
    Model planet((GLchar*)"planet.obj");        // ... Earth

    
    
    
    
    // =======================================================================
    // Step 3. Set the projection matrix
    // =======================================================================
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)sWidth/(GLfloat)sHeight,
                                            1.0f, 10000.0f);
    planetShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(planetShader.Program, "projection"),
                       1, GL_FALSE, glm::value_ptr(projection));

    
    
    
    
    

    // Iterate this block while the window is open
    while(!glfwWindowShouldClose(window))
    {

        // Check and call events
        glfwPollEvents();

        // Clear buffers
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        // Add transformation matrices
    
    
        // =======================================================================
        // Step 4. create the View matrix
        // =======================================================================
        planetShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(planetShader.Program, "view"), 1,
                           GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
    
    
    

        // =======================================================================
        // Step 5. Create the model matrix (We'll call it "planetModel" in this case)
        // =======================================================================
        planetShader.Use();
        glm::mat4 planetModel = glm::mat4(1);
    
        //Modify the model matrix with scaling, translation, rotation, etc
        planetModel = glm::scale(planetModel, glm::vec3(200.0f));
        planetModel = glm::translate(planetModel, glm::vec3(planetX, planetY, planetZ));
        
        // Make it rotate around the y-axis
        planetAngle += 0.01;
        planetModel = glm::rotate(planetModel, planetAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    
    
    
    
    
        // =======================================================================
        // Step 6. Pass the Model matrix, "planetModel", to the shader as "model"
        // =======================================================================
        glUniformMatrix4fv(glGetUniformLocation(planetShader.Program, "model"), 1,
                           GL_FALSE, glm::value_ptr(planetModel));
    
    
    
    
        // =======================================================================
        // Step 7.  Draw the object.
        // =======================================================================
        planet.Draw(planetShader);

    
    
        
        // Swap the buffers
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}





//--------------------------------------------------------------------------
// CallBack functions definition
//--------------------------------------------------------------------------

#pragma region "User input"

// This function is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if((key == GLFW_KEY_HOME||key == GLFW_KEY_H) && action == GLFW_PRESS)
            {
                camX = 0.0f;                                // Move the camera
                camY = 200.0f;                              // to the "home"
                camZ = 1700.0f;                             // location
            }

        if((key == GLFW_KEY_RIGHT) && action == GLFW_PRESS) // Move the camera right
            camX += 100.0f;
    
        if((key == GLFW_KEY_LEFT) && action == GLFW_PRESS)  // Move the camera left
            camX -= 100.0f;

    
        if((key == GLFW_KEY_UP) && action == GLFW_PRESS)    // Move the camera up
            camY += 100.0f;
    
        if((key == GLFW_KEY_DOWN) && action == GLFW_PRESS)  // Move the camera down
            camY -= 100.0f;

    
        if((key == GLFW_KEY_Z) && action == GLFW_PRESS)     // Move the camera forward
            camZ -= 100.0f;
    
        if((key == GLFW_KEY_O) && action == GLFW_PRESS)     // Move the camera backward
            camZ += 100.0f;

        // Reposition the camera
        camera = glm::vec3(camX, camY, camZ);

    
       // Acknowledge and reset the keyboard key which was pressed
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;

    }






// ----------------------------------------------------------------------
// Whenever the mouse moves, this callback is called. Moves camera view.
// ----------------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        GLfloat xoffset = xpos - lastX;
        GLfloat yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);

    }





// ----------------------------------------------------------------------
// Whenever the mouse buttons are clicked, this callback is called
// ----------------------------------------------------------------------
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        GLdouble xVal, yVal;

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            {
                glfwGetCursorPos(window, &xVal, &yVal);
                planetX = (xVal - 400) / 50;                 // NB Arbitrary equations
                planetY = (400 - yVal) / 50;                 // Not fixed equations
            }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
            {
                double xpos, ypos;
                //getting cursor position
                glfwGetCursorPos(window, &xpos, &ypos);
                cout << "Right Click: Cursor Position at (" << xpos << " : " << ypos << ")" << endl;
                planetX = xpos;
                
            }

    }




void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        camera.ProcessMouseScroll(yoffset);
    }



#pragma endregion
