#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/maths.hpp>
#include <common/camera.hpp>
#include <common/model.hpp>
#include <common/light.hpp>

// Function prototypes
void keyboardInput(GLFWwindow* window);
void mouseInput(GLFWwindow* window);
void updateJump();

// Frame timers
float previousTime = 0.0f;  // time of previous iteration of the loop
float deltaTime = 0.0f;  // time elapsed since the previous frame

// Create camera object
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f));


//global variables
bool isJumping = false;
float jumpTimer = 0.0f;
bool spacePressedLastFrame = false;
float baseMoveSpeed = 5.0f;
float sprintMultiplier = 1.5f;
float baseFOV = 45.0f;
float sprintFOV = 60.0f;

// Constants
const float jumpDuration = 1.0f;
const float jumpHeight = 1.5f;



// Object struct
struct Object
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    float angle = 0.0f;
    std::string name;
};

std::vector<Object> objects; 

int main(void)
{
    // =========================================================================
    // Window creation - you shouldn't need to change this code
    // -------------------------------------------------------------------------
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window;
    window = glfwCreateWindow(1024, 768, "Coursework", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    // -------------------------------------------------------------------------
    // End of window creation
    // =========================================================================

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Use back face culling
    glEnable(GL_CULL_FACE);

    // Ensure we can capture keyboard inputs
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Capture mouse inputs
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Compile shader program
    unsigned int shaderID, lightShaderID;
    shaderID = LoadShaders("vertexShader.glsl", "fragmentShader.glsl");
    lightShaderID = LoadShaders("lightVertexShader.glsl", "lightFragmentShader.glsl");

    // Activate shader
    glUseProgram(shaderID);

    // Load models
    Model teapot("../assets/teapot.obj");
    Model sphere("../assets/sphere.obj");
    Model cube("../assets/cube.obj");
    // Load the textures
    teapot.addTexture("../assets/blue.bmp", "diffuse");
    teapot.addTexture("../assets/diamond_normal.png", "normal");
    teapot.addTexture("../assets/neutral_specular.png", "specular");
    cube.addTexture("../assets/crate.jpg", "diffuse");
    cube.addTexture("../assets/diamond_normal.png", "normal");
    cube.addTexture("../assets/neutral_specular.png", "specular");
    // Define object lighting properties
    teapot.ka = 0.2f;
    teapot.kd = 0.7f;
    teapot.ks = 1.0f;
    teapot.Ns = 20.0f;
    cube.ka = 0.2f;
    cube.kd = 0.7f;
    cube.ks = 1.0f;
    cube.Ns = 20.0f;

    float teapotYaw = 0.0f;
    bool wasInProximityLastFrame = false;



    
    
    // Add light sources =======================================================
    Light lightSources;
    
    lightSources.addSpotLight(glm::vec3(0.0f, 3.0f, 0.0f),          // position
        glm::vec3(0.0f, -1.0f, 0.0f),         // direction
        glm::vec3(1.0f, 0.0f, 0.0f),          // colour
        1.0f, 0.1f, 0.02f,                    // attenuation
        std::cos(Maths::radians(39.0f)));     // cos(phi)

   

    lightSources.addPointLight(glm::vec3(-6.0f, 4.0f, -6.0f),        // position
        glm::vec3(0.5f, 0.5f, 0.5f),         // colour
        1.0f, 0.3f, 0.1f);                  // attenuation
    lightSources.addPointLight(glm::vec3(6.0f, 4.0f, -6.0f),        // position
        glm::vec3(0.5f, 0.5f, 0.5f),         // colour
        1.0f, 0.3f, 0.1f);                   // attenuation
    lightSources.addPointLight(glm::vec3(6.0f, 4.0f, 6.0f),        // position
        glm::vec3(0.5f, 0.5f, 0.5f),         // colour
        1.0f, 0.3f, 0.1f);                // attenuation
    lightSources.addPointLight(glm::vec3(-6.0f, 4.0f, 6.0f),        // position
        glm::vec3(0.5f, 0.5f, 0.5f),         // colour
        1.0f, 0.3f, 0.1f);                // attenuation

    // Teapot positions ==========================================================
    glm::vec3 teapotPositions[] = {
        glm::vec3(0.0f,  0.3f,  0.0f),
    };
    glm::vec3 boxPositions[] = {
       glm::vec3(2.0f,  -0.5f,  2.0f),
       glm::vec3(-2.0f,  -0.5f,  2.0f),
       glm::vec3(2.0f,  -0.5f,  -2.0f),
       glm::vec3(-2.0f,  -0.5f,  -2.0f),
    };

    // Add objects to objects vector
    
    Object object;
    object.name = "teapot";
    for (unsigned int i = 0; i < 1; i++)
    {
        object.position = teapotPositions[i];
        object.rotation = glm::vec3(1.0f, 1.0f, 1.0f);
        object.scale = glm::vec3(0.5f, 0.5f, 0.5f);
        object.angle = 0.0f;
        objects.push_back(object);
    }
    object.name = "cube";
    for (unsigned int i = 0; i < 4; i++)
    {
        object.position = boxPositions[i];
        object.rotation = glm::vec3(1.0f, 1.0f, 1.0f);
        object.scale = glm::vec3(0.4f, 0.4f, 0.4f);
        object.angle = 0.0f;
        objects.push_back(object);
    }
    object.position = glm::vec3(0.0f, -0.5f, 0.0f);
    object.scale = glm::vec3(0.4f, 0.4f, 0.4f);
    objects.push_back(object);

    // Load a 2D plane model for the floor and add textures =================================
    Model floor("../assets/plane.obj");
    floor.addTexture("../assets/stones_diffuse.png", "diffuse");
    floor.addTexture("../assets/stones_normal.png", "normal");

    // Define floor light properties
    floor.ka = 0.2f;
    floor.kd = 1.0f;
    floor.ks = 1.0f;
    floor.Ns = 20.0f;

    // Add floor model to objects vector
    object.position = glm::vec3(0.0f, -0.85f, 0.0f);
    object.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    object.rotation = glm::vec3(0.0f, 1.0f, 0.0f);
    object.angle = 0.0f;
    object.name = "floor";
    objects.push_back(object);
    object.position = glm::vec3(0.0f, 10.0f, 0.0f);
    object.rotation = glm::vec3(1.0f, 0.0f, 0.0f);
    object.angle = Maths::radians(180.0f);
    objects.push_back(object);

    //create a wall ===========================================================
    Model wall("../assets/plane.obj");
    wall.addTexture("../assets/bricks_diffuse.png", "diffuse");
    wall.addTexture("../assets/bricks_normal.png", "normal");
    wall.addTexture("../assets/bricks_specular.png", "specular");
    // Define wall light properties
    wall.ka = 0.2f;
    wall.kd = 1.0f;
    wall.ks = 1.0f;
    wall.Ns = 20.0f;
    glm::vec3 wallPositions[4] = {
        glm::vec3(0.0f,  0.5f, -10.0f),  //right wall
        glm::vec3(0.0f,  0.5f, 10.0f),   // left
        glm::vec3(10.0f, 0.5f, 0.0f), //front
        glm::vec3(-10.0f, 0.5f, 0.0f),
    };

    glm::vec3 wallRotate[4] = {
        glm::vec3(1.0f, 0.0f, 0.0f),  // right wall
        glm::vec3(-1.0f, 0.0f, 0.0f), // left
        glm::vec3(0.0f, 0.0f, 2.0f), //front]
        glm::vec3(0.0f, 0.0f, 1.0f), //back
    };

    float wallAngle[4] = {
        Maths::radians(90.0f), // right
        Maths::radians(90.0f), //left
        Maths::radians(90.0f), //front 
        Maths::radians(-90.0f),
    };

    for (int i = 0; i < 4; i++) {
        object.position = wallPositions[i];
        object.rotation = glm::vec3(1.0f, 0.0f, 0.0f);  // rotate around X to stand upright
        object.angle = 90.0f;                       
        object.scale = glm::vec3(10.0f, 10.0f, 10.0f);   // make wall big
        object.name = "wall";

       
        object.rotation = wallRotate[i];  // rotate around Y to face inward
        object.angle = wallAngle[i];

        objects.push_back(object);
    }


    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Update timer
        float time = glfwGetTime();
        deltaTime = time - previousTime;
        previousTime = time;

        // Get inputs
        keyboardInput(window);
        mouseInput(window);
        updateJump();

        // Clear the window
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate view and projection matrices
        camera.target = camera.eye + camera.front;
        camera.quaternionCamera();

        // Activate shader
        glUseProgram(shaderID);

        float proximityThreshold = 3.0f;
        float distanceToOrigin = glm::length(camera.eye - glm::vec3(0.0f, 0.0f, 0.0f));
        bool isInProximity = distanceToOrigin < proximityThreshold;

        // Flashing light colours
        for (auto& light : lightSources.lightSources) {
            if (light.type == 2) {
                light.colour = glm::vec3(1.0f, 1.0f, 0.0f); // spotlight stays yellow
            }
            else if (isInProximity) {
                float t = (sin(glfwGetTime() * 5.0f) + 1.0f) / 2.0f;
                light.colour = glm::mix(glm::vec3(1, 0, 0), glm::vec3(0, 0, 1), t);
            }
            else {
                light.colour = glm::vec3(1.0f, 1.0f, 1.0f); // default white
            }
        }

        // Only rotate if in proximity
        if (isInProximity) {
            teapotYaw += deltaTime * 10.0f; // 10 degrees per second
            if (teapotYaw > 360.0f)
                teapotYaw -= 360.0f;
        }

        wasInProximityLastFrame = isInProximity;



        // Send light source properties to the shader
        lightSources.toShader(shaderID, camera.view);

        // Loop through objects
        for (unsigned int i = 0; i < static_cast<unsigned int>(objects.size()); i++)
        {
            // Calculate model matrix
            glm::mat4 translate = Maths::translate(objects[i].position);
            glm::mat4 scale = Maths::scale(objects[i].scale);
            glm::mat4 rotate = Maths::rotate(objects[i].angle, objects[i].rotation);
            glm::mat4 model = translate * rotate * scale;

            // Send the MVP and MV matrices to the vertex shader
            glm::mat4 MV = camera.view * model;
            glm::mat4 MVP = camera.projection * MV;
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "MV"), 1, GL_FALSE, &MV[0][0]);

            // Draw the model
            if (objects[i].name == "teapot") {
                Quaternion spinQuat(0.0f, teapotYaw); // spin around Y
                glm::mat4 spinMatrix = spinQuat.matrix();

                glm::mat4 translate = Maths::translate(objects[i].position);
                glm::mat4 scale = Maths::scale(objects[i].scale);
                glm::mat4 model = translate * spinMatrix * scale;

                glm::mat4 MV = camera.view * model;
                glm::mat4 MVP = camera.projection * MV;
                glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
                glUniformMatrix4fv(glGetUniformLocation(shaderID, "MV"), 1, GL_FALSE, &MV[0][0]);

                teapot.draw(shaderID);
            }


            if (objects[i].name == "floor")
                floor.draw(shaderID);
            if (objects[i].name == "wall")
                wall.draw(shaderID);
            if (objects[i].name == "cube")
                cube.draw(shaderID);
        }

        // Draw light sources
        lightSources.draw(lightShaderID, camera.view, camera.projection, sphere);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    teapot.deleteBuffers();
    glDeleteProgram(shaderID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    return 0;
}




void keyboardInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec3 proposedMove(0.0f);
    float moveSpeed = baseMoveSpeed;

    bool isSprinting = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    // Sprinting speed
    if (isSprinting)
        moveSpeed *= sprintMultiplier;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        proposedMove += moveSpeed * deltaTime * camera.front;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        proposedMove -= moveSpeed * deltaTime * camera.front;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        proposedMove -= moveSpeed * deltaTime * camera.right;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        proposedMove += moveSpeed * deltaTime * camera.right;
    // Disable vertical movement (force y = 0)
    proposedMove.y = 0.0f;

    glm::vec3 newPosition = camera.eye + proposedMove;

    // Clamp movement within walls at x/z ±10
    newPosition.x = glm::clamp(newPosition.x, -9.5f, 9.5f);
    newPosition.z = glm::clamp(newPosition.z, -9.5f, 9.5f);

    // Collision detection
    for (const Object& obj : objects) {
        float distance = glm::length(newPosition - obj.position);
        if (distance < 1.0f) {
            glm::vec3 pushDir = glm::normalize(newPosition - obj.position);
            newPosition = obj.position + pushDir * 1.0f;
        }
    }
    camera.eye = newPosition;
    // Update FOV for sprint effect
    camera.fov = isSprinting ? sprintFOV : baseFOV;

    // Jumping logic
    bool spacePressedNow = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (spacePressedNow && !spacePressedLastFrame && !isJumping)
    {
        isJumping = true;
        jumpTimer = 0.0f;
    }

    spacePressedLastFrame = spacePressedNow;
}


void updateJump()
{
    if (isJumping) {
        jumpTimer += deltaTime;
        if (jumpTimer >= jumpDuration) {
            jumpTimer = jumpDuration;
            isJumping = false;
        }
        float t = jumpTimer / jumpDuration;
        float height = jumpHeight * std::sin(glm::pi<float>() * t);
        camera.eye.y = height;
    }
    else {
        float baseHeight = 0.0f;
        for (const Object& obj : objects) {
            // Check if we're above this cube and within XZ bounds
            glm::vec3 diff = camera.eye - obj.position;

            // Simple bounding box check
            if (std::abs(diff.x) < 0.5f && std::abs(diff.z) < 0.5f) {
                float top = obj.position.y + obj.scale.y;
                if (std::abs(camera.eye.y - top) < 0.3f) { // close to top
                    baseHeight = top;
                    break;
                }
            }
        }

        camera.eye.y = baseHeight;
    }

}

void mouseInput(GLFWwindow* window)
{
    // Get mouse cursor position and reset to centre
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Update yaw and pitch angles
    camera.yaw += 0.005f * float(xPos - 1024 / 2);
    camera.pitch += 0.005f * float(768 / 2 - yPos);

    // Calculate camera vectors from the yaw and pitch angles
    camera.calculateCameraVectors();
}

