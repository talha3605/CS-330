#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "SHADER.H"

#include <iostream>

#include "cylinder.h"





void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(-3.5f, 0.0f, -3.7f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Milestone", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("shaderfiles/6.multiple_lights.vs", "shaderfiles/6.multiple_lights.fs");
    Shader lightCubeShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");

    float vertices[] = {
        // positions          //normal       // texture coords
        // plane counter top
        -5.0f, -5.0f, -5.0f, 0.0f,  0.0f, -1.0f,       0.0f, 1.0f, // top left
        -5.0f, -5.0f,  5.0f, 0.0f,  0.0f, -1.0f,       0.0f, 0.0f, // bottom left
         5.0f, -5.0f,  5.0f, 0.0f,  0.0f, -1.0f,       1.0f, 0.0f, // bottom right
         5.0f, -5.0f, -5.0f, 0.0f,  0.0f, -1.0f,       1.0f, 1.0f, // top right

         //Cell Phone Body

         2.2f, -4.99f,  0.5f, 0.0f,  0.0f, -1.0f,       0.0f, 1.0f, // top left
         1.8f, -4.99f,  3.5f, 0.0f,  0.0f, -1.0f,       0.0f, 0.0f, // bottom left
         2.8f, -4.99f,  3.5f, 0.0f,  0.0f, -1.0f,      1.0f, 0.0f, // bottom right
         3.2f, -4.99f,  0.5f, 0.0f,  0.0f, -1.0f,       1.0f, 1.0f, // top right

         2.2f, -4.7f,  0.5f, 0.0f,  0.0f, -1.0f,       0.0f, 1.0f, // top left
         1.8f, -4.7f,  3.5f, 0.0f,  0.0f, -1.0f,       0.0f, 0.0f, // bottom left
         2.8f, -4.7f,  3.5f, 0.0f,  0.0f, -1.0f,       1.0f, 0.0f, // bottom right
         3.2f, -4.7f,  0.5f, 0.0f,  0.0f, -1.0f,       1.0f, 1.0f, // top right

         //Cell Phone Screen

         2.25f, -4.7f,  0.55f, 0.0f,  0.0f, -1.0f,       0.0f, 1.0f, // top left
         1.85f, -4.7f,  3.45f, 0.0f,  0.0f, -1.0f,       0.0f, 0.0f, // bottom left
         2.75f, -4.7f,  3.45f, 0.0f,  0.0f, -1.0f,       1.0f, 0.0f, // bottom right
         3.15f, -4.7f,  0.55f, 0.0f,  0.0f, -1.0f,       1.0f, 1.0f, // top right

         //tissue box

        -4.0f, -4.99f, -3.0f, 0.0f,  0.0f, -1.0f,       0.0f, 1.0f, // top left
        -3.0f, -4.99f,  0.0f, 0.0f,  0.0f, -1.0f,       0.0f, 0.0f, // bottom left
         1.0f, -4.99f, -1.0f, 0.0f,  0.0f, -1.0f,       1.0f, 0.0f, // bottom right
         0.0f, -4.99f, -4.0f, 0.0f,  0.0f, -1.0f,       1.0f, 1.0f, // top right

        -4.0f, -4.0f, -3.0f, 0.0f,  0.0f, -1.0f,       0.0f, 1.0f, // top left
        -3.0f, -4.0f,  0.0f, 0.0f,  0.0f, -1.0f,       0.0f, 0.0f, // bottom left
         1.0f, -4.0f, -1.0f, 0.0f,  0.0f, -1.0f,       1.0f, 0.0f, // bottom right
         0.0f, -4.0f, -4.0f, 0.0f,  0.0f, -1.0f,       1.0f, 1.0f, // top right

         //tissue box hole

        -2.5f, -3.9f,  -2.05f, 0.0f,  0.0f, -1.0f,     0.0f, 1.0f, // top left
        -2.25f, -3.9f, -1.30f, 0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // bottom left
        -0.65f, -3.9f, -1.75f, 0.0f,  0.0f, -1.0f,     1.0f, 0.0f, // bottom right
        -0.90f, -3.9f, -2.50f, 0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // top right

        //foam brush head #1

        -1.5f, -4.99f,  1.5f, 0.0f,  0.0f, -1.0f,       0.0f, 1.0f, // top left
        -1.0f, -4.99f,  1.95f, 0.0f,  0.0f, -1.0f,      0.0f, 0.0f, // bottom left
        -0.5f, -4.99f,  1.1f, 0.0f,  0.0f, -1.0f,       1.0f, 0.0f, // bottom right
        -0.95f, -4.99f, 0.55f, 0.0f,  0.0f, -1.0f,      1.0f, 1.0f, // top right

        -1.5f, -4.65f,  1.5f, 0.0f,  0.0f, -1.0f,      0.0f, 1.0f, // top left
        -1.0f,  -4.65f,  1.95f, 0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // bottom left
        -0.64f,  -4.65f, 1.31f, 0.0f,  0.0f, -1.0f,      1.0f, 0.0f, // bottom right
        -1.05f, -4.65f,  0.85f, 0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // top right

        -1.0f, -4.99f,  1.95f, 0.0f,  0.0f, -1.0f,      0.0f, 0.0f, // bottom left
        -0.5f, -4.99f,  1.1f,  0.0f,  0.0f, -1.0f,      1.0f, 0.0f, // bottom right
        -1.0f,  -4.65f,  1.95f, 0.0f,  0.0f, -1.0f,     0.0f, 1.0f, // bottom left
        -0.64f,  -4.65f, 1.31f, 0.0f,  0.0f, -1.0f,      1.0f, 1.0f, // bottom right

        -0.5f, -4.99f,  1.1f, 0.0f,  0.0f, -1.0f,       1.0f, 0.0f, // bottom right
        -0.95f, -4.99f, 0.55f, 0.0f,  0.0f, -1.0f,      1.0f, 1.0f, // top right
        -0.64f,  -4.65f, 1.31f, 0.0f,  0.0f, -1.0f,    0.0f, 0.0f, // bottom right
        -1.05f, -4.65f,  0.85f, 0.0f,  0.0f, -1.0f,    0.0f, 1.0f, // top right

        -1.5f, -4.99f,  1.5f, 0.0f,  0.0f, -1.0f,       0.0f, 0.0f, // top left
        -1.0f, -4.99f,  1.95f, 0.0f,  0.0f, -1.0f,      1.0f, 0.0f, // bottom left
        -1.5f, -4.65f,  1.5f, 0.0f,  0.0f, -1.0f,      0.0f, 1.0f, // top left
        -1.0f,  -4.65f,  1.95f, 0.0f,  0.0f, -1.0f,    1.0f, 1.0f, // bottom left

        -1.5f, -4.99f,  1.5f, 0.0f,  0.0f, -1.0f,       1.0f, 0.0f, // top left
        -1.5f, -4.65f,  1.5f, 0.0f,  0.0f, -1.0f,      1.0f, 1.0f, // top left
        -0.95f, -4.99f, 0.55f, 0.0f,  0.0f, -1.0f,      0.0f, 0.0f, // top right
        -1.05f, -4.65f,  0.85f, 0.0f,  0.0f, -1.0f,    0.0f, 1.0f, // top right

        //foam brush head #2

        -2.0f, -4.99f,  1.0f, 0.0f,  0.0f, -1.0f,       0.0f, 1.0f, // top left
        -1.5f, -4.99f,  1.45f, 0.0f,  0.0f, -1.0f,      0.0f, 0.0f, // bottom left
        -1.0f, -4.99f,  0.6f, 0.0f,  0.0f, -1.0f,       1.0f, 0.0f, // bottom right
        -1.45f, -4.99f, 0.05f, 0.0f,  0.0f, -1.0f,      1.0f, 1.0f, // top right

        -2.0f, -4.65f,   1.0f, 0.0f,  0.0f, -1.0f,      0.0f, 1.0f, // top left
        -1.5f,  -4.65f,  1.45f, 0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // bottom left
        -1.14f,  -4.65f, 0.81f, 0.0f,  0.0f, -1.0f,     1.0f, 0.0f, // bottom right
        -1.55f, -4.65f,  0.35f, 0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // top right

        -1.5f, -4.99f,  1.45f, 0.0f,  0.0f, -1.0f,      0.0f, 0.0f, // bottom left
        -1.0f, -4.99f,  0.6f, 0.0f,  0.0f, -1.0f,       1.0f, 0.0f, // bottom right
        -1.5f,  -4.65f,  1.45f, 0.0f,  0.0f, -1.0f,     0.0f, 1.0f, // bottom left
        -1.14f,  -4.65f, 0.81f, 0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // bottom right

        -1.0f, -4.99f,  0.6f, 0.0f,  0.0f, -1.0f,       1.0f, 0.0f, // bottom right
        -1.45f, -4.99f, 0.05f, 0.0f,  0.0f, -1.0f,      1.0f, 1.0f, // top right
        -1.14f,  -4.65f, 0.81f, 0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // bottom right
        -1.55f, -4.65f,  0.35f, 0.0f,  0.0f, -1.0f,     0.0f, 1.0f, // top right

        -2.0f, -4.99f,  1.0f, 0.0f,  0.0f, -1.0f,       0.0f, 0.0f, // top left
        -1.5f, -4.99f,  1.45f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom left
        -2.0f, -4.65f,   1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top left
        -1.5f,  -4.65f,  1.45f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // bottom left

        -2.0f, -4.99f,  1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // top left
        -2.0f, -4.65f,   1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top left
        -1.45f, -4.99f, 0.05f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // top right
        -1.55f, -4.65f,  0.35f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top right

        //Pen Tip

       -0.92f, -4.95f,  2.22f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
       -0.82f, -4.998f,   2.27f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
       -0.87f, -4.998f,  2.34f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

       -0.92f, -4.95f, 2.22f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
       -0.82f,  -4.92f, 2.27f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
       -0.87f,  -4.92f, 2.34f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

       //Duct tape

       2.1f, -4.48f, -2.2f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
       3.5f, -4.48f, -0.8f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
       3.5f, -4.48f, -2.2f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
       2.1f, -4.48f, -0.8f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

       //light cube

       -0.5f, -3.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -3.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f, -2.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f, -2.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
       -0.5f, -2.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
       -0.5f, -3.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

       -0.5f, -3.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -3.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f, -2.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f, -2.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
       -0.5f, -2.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
       -0.5f, -3.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

       -0.5f, -2.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
       -0.5f, -2.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
       -0.5f, -3.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
       -0.5f, -3.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
       -0.5f, -3.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
       -0.5f, -2.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        0.5f, -2.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -2.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -3.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -3.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -3.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f, -2.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

       -0.5f, -3.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -3.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -3.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -3.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
       -0.5f, -3.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
       -0.5f, -3.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

       -0.5f, -2.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -2.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -2.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -2.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
       -0.5f, -2.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
       -0.5f, -2.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f

    };

    unsigned int indices[] = {

        //counter top
        0,1,2,
        0,2,3,

        //cell phone body
        4,5,6,
        4,6,7,
        4,8,9,
        4,5,9,
        4,7,8,
        7,8,11,
        5,6,9,
        6,9,10,
        6,7,11,
        6,10,11,

        //cell phone screen
        12,13,14,
        12,14,15,

        //tissue box
        16,17,18,
        16,18,19,
        20,21,22,
        20,22,23,
        16,19,20,
        19,20,23,
        16,17,20,
        17,20,21,
        17,18,21,
        18,21,22,
        18,19,22,
        19,22,23,

        //tissue box hole
        24,25,26,
        24,26,27,

        //foam brush head #1
        28,29,30,
        28,30,31,
        32,33,34,
        32,34,35,
        36,37,38,
        37,38,39,
        40,41,42,
        41,42,43,
        44,45,46,
        45,46,47,
        48,49,50,
        49,50,51,

        //foam brush head #2
        52,53,54,
        52,54,55,
        56,57,58,
        56,58,59,
        60,61,62,
        61,62,63,
        64,65,66,
        65,66,67,
        68,69,70,
        69,70,71,
        72,73,74,
        73,74,75,

        //pen tip
        76,77,78,
        79,80,81,
        76,78,81,
        79,77,80,

        //duct tape
        82,83,85,
        82,83,84,

    };

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(4.0f,  -4.0f,  -4.0f),
        glm::vec3(-4.0f, -4.0f,  4.0f),
        glm::vec3(4.0f,  -4.0f, 4.0f),
        glm::vec3(-4.0f,  -4.0f, -4.0f)
    };

    unsigned int VBO, VAO, EBO;
    unsigned int VBO2, VAO2;
    unsigned int VBO3, VAO3;


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // position attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);


    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    unsigned int diffuseMap = loadTexture("images/tile.jpg");
    unsigned int specularMap = loadTexture("images/tile.jpg");

    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    // load and create a texture 
       // -------------------------
    unsigned int texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9;

    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("images/GIC1.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 2
// ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("images/IS.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 3
// ---------
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("images/pb1.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 4
// ---------
    glGenTextures(1, &texture4);
    glBindTexture(GL_TEXTURE_2D, texture4);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("images/foam.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 5
// ---------
    glGenTextures(1, &texture5);
    glBindTexture(GL_TEXTURE_2D, texture5);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("images/silver1.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 6
// ---------
    glGenTextures(1, &texture6);
    glBindTexture(GL_TEXTURE_2D, texture6);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("images/black.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 7
// ---------
    glGenTextures(1, &texture7);
    glBindTexture(GL_TEXTURE_2D, texture7);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("images/white.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 8
// ---------
    glGenTextures(1, &texture8);
    glBindTexture(GL_TEXTURE_2D, texture8);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("images/wood.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 9
// ---------
    glGenTextures(1, &texture9);
    glBindTexture(GL_TEXTURE_2D, texture9);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    data = stbi_load("images/ducttape.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);



    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("texture1", 2);
    lightingShader.setInt("texture2", 3);
    lightingShader.setInt("texture3", 4);
    lightingShader.setInt("texture4", 5);
    lightingShader.setInt("texture5", 6);
    lightingShader.setInt("texture6", 7);
    lightingShader.setInt("texture7", 8);
    lightingShader.setInt("texture8", 9);
    lightingShader.setInt("texture9", 10);


    glm::mat4 model;
    float angle;

    //

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // light on the plane for the tile

        // activate shader
        lightingShader.use();
        lightingShader.setVec3("viewPos", cameraPos);
        lightingShader.setFloat("material.shininess", 32.0f);

        lightingShader.setVec3("dirLight.direction", 0.0f, -5.0f, 0.0f);
        lightingShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.8f, 0.8f, 0.8f);
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", 0.3f, 0.3f, 0.3f);
        lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", 0.3f, 0.3f, 0.3f);
        lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", 0.3f, 0.3f, 0.3f);
        lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", 0.3f, 0.3f, 0.3f);
        lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032);
        // spotLight
        lightingShader.setVec3("spotLight.position", cameraPos);
        lightingShader.setVec3("spotLight.direction", cameraFront);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09);
        lightingShader.setFloat("spotLight.quadratic", 0.032);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        bool switchProjection = false;
        glm::mat4 projection = switchProjection ?
            projection = glm::perspective(glm::radian(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f) :
            projection = glm::ortho(-(float)SCR_WIDTH, (float)SCR_WIDTH, -(float)SCR_HEIGHT, (float)SCR_HEIGHT, 0.1f, 100.0f);

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
            switchProjection == true;
            projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
        }
        else {
            projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        }


        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        lightingShader.setMat4("view", view);


        glBindVertexArray(VAO);
        // create transformations
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        angle = 0.0f;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glBindVertexArray(VAO);

        // create transformations
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        angle = 0.0f;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, 38, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO);

        // create transformations
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        angle = 0.0f;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glBindVertexArray(VAO);

        // create transformations
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        angle = 0.0f;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, 80, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO);

        // create transformations
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        angle = 0.0f;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, 84, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture4);
        glBindVertexArray(VAO);

        // create transformations
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        angle = 0.0f;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, 158, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture5);
        glBindVertexArray(VAO);

        // create transformations
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        angle = 0.0f;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, 170, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture9);
        glBindVertexArray(VAO);

        // create transformations
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        angle = 0.0f;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, 200, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glBindVertexArray(VAO2);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.74f, 0.0f));
        lightingShader.setMat4("model", model);

        static_meshes_3D::Cylinder C(0.1, 100, 0.5, true, true, true);
        C.render();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture7);
        glBindVertexArray(VAO2);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.39f, 0.0f));
        lightingShader.setMat4("model", model);

        static_meshes_3D::Cylinder C2(0.1, 100, 0.2, true, true, true);
        C2.render();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glBindVertexArray(VAO2);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, -1.8f, 1.63f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.2f, 0.0f, 0.8f));
        lightingShader.setMat4("model", model);

        static_meshes_3D::Cylinder C3(0.07, 100, 1.0, true, true, true);
        C3.render();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture8);
        glBindVertexArray(VAO2);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.48f, -1.8f, 2.11f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.2f, 0.0f, 0.8f));
        lightingShader.setMat4("model", model);

        static_meshes_3D::Cylinder C4(0.07, 100, 1.0, true, true, true);
        C4.render();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glBindVertexArray(VAO2);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.5f, -1.96f, 2.65f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-0.2f, 0.0f, 0.2f));
        lightingShader.setMat4("model", model);

        static_meshes_3D::Cylinder C5(0.04, 100, 1.0, true, true, true);
        C5.render();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture6);
        glBindVertexArray(VAO2);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.8f, -1.74f, -1.5f));
        lightingShader.setMat4("model", model);

        static_meshes_3D::Cylinder C6(1, 100, 0.5, true, true, true);
        C6.render();

        // create transformations
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        lightingShader.setMat4("model", model);

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 86, 36);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &EBO);



    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID; 
}
