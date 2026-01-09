#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "camera.hpp"
#include "model.hpp"
#include "shader.hpp"

#define WIDTH 800
#define HEIGHT 600

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos); 
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const * path);

// timekeeping
float deltaTime = 0.0f; // time between current and last frame
float lastFrame = 0.0f; // time from start to last frame being rendered

// camera
const glm::vec3 initCameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
const glm::vec3 initCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 initCameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

Camera camera(initCameraPos, initCameraFront, initCameraUp, WIDTH, HEIGHT);

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) 
    {
        std::cout << "Failed to create GLFW window" << '\n';
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialise GLAD" << '\n';
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    Shader modelShader("shaders/model.vert", "shaders/model.frag");

    std::string objDirPath = "resources/objects/";
    std::string backpack = "backpack/backpack.obj";
    std::string buddha = "buddha/buddha.obj";
    std::string bunny = "bunny/bunny.obj";
    std::string dragon = "dragon/dragon.obj";
    std::string sponza = "sponza/sponza.obj";
    std::string statuette = "statuette/statuette.ply";
    //Model modelObj(objDirPath + backpack);
    Model modelObj(objDirPath + buddha);

    modelShader.use();
    modelShader.setInt("material.diffuse", 0);
    modelShader.setInt("material.specular", 0);
    modelShader.setFloat("material.shininess", 32.0f);

    modelShader.setVec3("dirLight.direction", glm::vec3(-1.0f, -1.0f, -1.0f));
    modelShader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    modelShader.setVec3("dirLight.diffuse", glm::vec3(0.4, 0.4f, 0.4f));
    modelShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    while (!glfwWindowShouldClose(window)) {
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // rendering commands here
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // for the non source cube
        modelShader.use();

        // create transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), 
                (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        float modelScale = 1.0f;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(modelScale));

        // vert shader
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        modelShader.setMat4("model", model);

        // frag shader
        modelShader.setVec3("viewPos", camera.pos);
        modelObj.Draw(modelShader);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
  
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera.ProcessMouse(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessScroll(yoffset);
}

unsigned int loadTexture(char const * path) {
    
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;

    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    
    //std::cout << width << " " << height << " " << nrChannels << '\n';

    if (data) {
        GLenum format = GL_RED;
        if (nrChannels == 1) {
            format = GL_RED;
        } else if (nrChannels == 3) {
            format = GL_RGB;
        } else if (nrChannels == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, 
                GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load texture from path: " << path << '\n';
    }

    stbi_image_free(data);
    return textureID;
}
