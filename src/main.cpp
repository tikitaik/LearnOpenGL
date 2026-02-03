#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "camera.hpp"
#include "model.hpp"
#include "shader.hpp"

#define WIDTH 1280
#define HEIGHT 720

#define ASTEROID_AMOUNT 100000

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos); 
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const * path, bool isSRGB);
unsigned int loadCubemap(std::vector<std::string> faces);

// custom rendering functions
void renderScene(Shader shader, Model shadowTheHedgehog); 
void renderFrameBufferToScreen(Shader screenQuadShader);

// custom silly functions
void getVAOS();
void getTangents(const unsigned int rowSize, const unsigned int triangleCount, float* vertices, float* tangents);
std::string getBuildPath(std::string command);
void getAsteroidTranslations(glm::mat4 asteroidTranslations[ASTEROID_AMOUNT]);

// timekeeping
float deltaTime = 0.0f; // time between current and last frame
float lastFrame = 0.0f; // time from start to last frame being rendered

// camera
const glm::vec3 initCameraPos   = glm::vec3(0.0f,  3.0f,  4.0f);
const glm::vec3 initCameraFront = glm::vec3(0.0f, -0.5f, -1.0f);
const glm::vec3 initCameraUp    = glm::vec3(0.0f,  1.0f,  0.0f);

// object VAOs
unsigned int quadVAO, quadVBO;
unsigned int planeVAO, planeVBO, planeTangentsVBO;
unsigned int cubeVAO, cubeVBO, cubeTangentsVBO;
unsigned int planeTexture, planeNormalTexture, planeDispTexture;
unsigned int cubeTexture, cubeNormalTexture, cubeDispTexture;

unsigned int screenFBO;
unsigned int screenTexture;
unsigned int screenRBO;

unsigned int cameraMatrixBlock;

Camera camera(initCameraPos, initCameraFront, initCameraUp, WIDTH, HEIGHT);

int main(int argc, char* argv[])
{
    // glfw initialise
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "heheheheheh :)))", NULL, NULL);
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

    // load opengl function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialise GLAD" << '\n';
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    // ---------------------- //
    // Configure OpenGL State //
    // ---------------------- //
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // ------------ //
    // Shader Setup //
    // ------------ //
    std::string buildPath = getBuildPath(std::string(argv[0]));
    std::string shaderPath = buildPath + "shaders/";

    glm::vec3 lightPos(0.0f, 5.0f, 0.0f);

    float far_plane = 25.0f;

    // back to boring setup stuff now
    Shader blinnphongShader(buildPath, "blinnphong");
    Shader bloomShader(buildPath, "bloom");
    Shader bloomBlendShader(buildPath, "bloomblend");
    Shader depthMapShader(buildPath, "depthmap");
    Shader gaussBlurShader(buildPath, "gaussblur");
    Shader lightBoxShader(buildPath, "lightbox");
    Shader hdrScreenShader(buildPath, "hdr");
    Shader modelShader(buildPath, "model");
    Shader normalMapShader(buildPath, "normalmap");
    Shader parallaxShader(buildPath, "parallax");
    Shader screenQuadShader(buildPath, "screenquad");

    // bruh ass uniforms
    blinnphongShader.use();
    blinnphongShader.setInt("diffuseMap", 0);
    blinnphongShader.setVec3("lightPos", lightPos);

    hdrScreenShader.use();
    hdrScreenShader.setInt("tex", 0);

    modelShader.use();
    modelShader.setInt("material.diffuse", 0);
    modelShader.setInt("material.specular", 0);
    modelShader.setFloat("material.shininess", 200.0f);
    modelShader.setInt("ourTex", 0);

    modelShader.setVec3("dirLight.direction", glm::vec3(1.0f, -1.0f, -1.0f));
    modelShader.setVec3("dirLight.ambient", glm::vec3(0.01f, 0.01f, 0.01f));
    modelShader.setVec3("dirLight.diffuse", glm::vec3(0.1f, 0.1f, 0.1f));
    modelShader.setVec3("dirLight.specular", glm::vec3(0.9f, 0.9f, 0.9f));

    normalMapShader.use();
    normalMapShader.setVec3("lightPos", lightPos);
    normalMapShader.setInt("diffuseMap", 0);
    normalMapShader.setInt("normalMap", 1);
    normalMapShader.setInt("depthMap", 2);
    normalMapShader.setFloat("far_plane", far_plane);

    parallaxShader.use();
    parallaxShader.setVec3("lightPos", lightPos);
    parallaxShader.setInt("diffuseMap", 0);
    parallaxShader.setInt("normalMap", 1);
    parallaxShader.setInt("depthMap", 2);
    parallaxShader.setFloat("height_scale", 0.1f);

    std::string objDirPath = buildPath + "resources/objects/";
    std::string backpackPath = "backpack/backpack.obj";
    std::string buddhaPath = "buddha/buddha.obj";
    std::string bunnyPath = "bunny/bunny.obj";
    std::string dragonPath = "dragon/dragon.obj";
    std::string sponzaPath = "sponza/sponza.obj";
    std::string planetPath = "planet/planet.obj";
    std::string rockPath = "rock/rock.obj";
    std::string shadowHedgehogPath = "shadow/scene.gltf";

    Model backpack(objDirPath + backpackPath);
    Model shadowTheHedgehog(objDirPath + shadowHedgehogPath);

    // -------------- //
    // BUFFER OBJECTS //
    // -------------- //

    getVAOS();

    glfwSwapInterval(0);

    // ------------- //
    // Load Textures //
    // ------------- //

    std::string texPath = buildPath + "resources/textures/";

    planeTexture = loadTexture((texPath + "brickwall.jpg").c_str(), true);
    planeNormalTexture = loadTexture((texPath + "bricks2_normal.jpg").c_str(), false);
    planeDispTexture = loadTexture((texPath + "bricks2_disp.jpg").c_str(), false);

    cubeTexture = loadTexture((texPath + "container2.png").c_str(), true);
    cubeNormalTexture = loadTexture((texPath + "bricks2_normal.jpg").c_str(), false);
    cubeDispTexture = loadTexture((texPath + "bricks2_disp.jpg").c_str(), false);

    // --------- //
    // Main Loop //
    // --------- //
    
    while (!glfwWindowShouldClose(window)) {
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 view = camera.GetViewMatrix();

        processInput(window);

        // load view matrix into memory
        glBindBuffer(GL_UNIFORM_BUFFER, cameraMatrixBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &view);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Actual Rendering //
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        blinnphongShader.use();
        blinnphongShader.setVec3("viewPos", camera.pos);
        blinnphongShader.setVec3("lightPos", glm::vec3(0.0f, 5.0f, 0.0f));

        renderScene(blinnphongShader, shadowTheHedgehog); 

        renderFrameBufferToScreen(hdrScreenShader);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // i think i need to delete all the things here
  
    glfwTerminate();
    return 0;
}

void renderScene(Shader shader, Model shadowTheHedgehog) {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, planeTexture);

    glm::mat4 model = glm::mat4(1.0f);

    shader.use();
    shader.setVec3("viewPos", camera.pos);
    shader.setInt("diffuseMap", 0);

    // create one large cube that acts as the floor
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);

    // then create multiple cubes as the scenery
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
    model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(1.25));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
    model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // render obj model
    model = glm::mat4(1.0f);
    model = translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
    model = glm::scale(model, glm::vec3(0.05f));
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);
    shadowTheHedgehog.Draw(shader);
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

unsigned int loadTexture(char const * path, bool isSRGB) {
    
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;

    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    
    //std::cout << width << " " << height << " " << nrChannels << '\n';

    GLenum internalFormat, dataFormat;

    if (data) {
        internalFormat = dataFormat = GL_RED;
        if (nrChannels == 1) {
            internalFormat = dataFormat = GL_RED;
        } else if (nrChannels == 3) {
            internalFormat = isSRGB ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        } else if (nrChannels == 4) {
            internalFormat = isSRGB ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, 
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

unsigned int loadCubemap(std::vector<std::string> faces) {

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    stbi_set_flip_vertically_on_load(false);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {

        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << '\n';
        }

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(true);

    return textureID;
}

void renderFrameBufferToScreen(Shader screenQuadShader)  {

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
}

void getTangents(const unsigned int rowSize,
        const unsigned int triangleCount, float* vertices, float* tangents) {

    for (unsigned int i = 0; i < triangleCount; i++) {

        int texOffset = 6;

        glm::vec3 tangent;

        glm::vec3 pos[3];
        glm::vec2 tex[3];

        float* firstCoord = &vertices[i * rowSize * 3];

        for (int j = 0; j < 3; j++) {

            pos[j] = glm::vec3(*(firstCoord), *(firstCoord + 1), *(firstCoord + 2));
            tex[j] = glm::vec2(*(firstCoord + texOffset), *(firstCoord + texOffset + 1));
            firstCoord += rowSize;
        }

        glm::vec3 edge1 = pos[1] - pos[0];
        glm::vec3 edge2 = pos[2] - pos[0];
        glm::vec2 deltaUV1 = tex[1] - tex[0];
        glm::vec2 deltaUV2 = tex[2] - tex[0];

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        // tangents for one triangle
        for (int j = 0; j < 3; j++) {

            tangents[i * 9 + j * 3 + 0] = tangent.x;
            tangents[i * 9 + j * 3 + 1] = tangent.y;
            tangents[i * 9 + j * 3 + 2] = tangent.z;

            //std::cout << "[" << tangent.x << ", " << tangent.y << ", " << tangent.z << "]\n";
        }
    }
}

void getVAOS() {

    float quadVertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    float planeVertices[] = {
        // positions           // normals        // textures
        -0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    float cubeVertices[] = {
         // positions        // normals         //texture coords
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    float planeTangents[6 * 3];
    float cubeTangents[36 * 3];
    getTangents(8, 2, planeVertices, planeTangents);
    getTangents(8, 12, cubeVertices, cubeTangents);

    // screen quad
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

     // for the floor
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glGenBuffers(1, &planeTangentsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeTangentsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeTangents), &planeTangents, GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // cube
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glGenBuffers(1, &cubeTangentsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeTangentsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTangents), &cubeTangents, GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenFramebuffers(1, &screenFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &screenRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, screenRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screenRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
    }

    // uniform buffer block
    glGenBuffers(1, &cameraMatrixBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraMatrixBlock);
    glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraMatrixBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // load the camera projection matrix into the uniform buffer object memory
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov),
            (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraMatrixBlock);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &projection);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

std::string getBuildPath(std::string argv_0) {

    // hehehe this will let us find executable location
    std::string resPath(argv_0);
    std::string cwd = std::filesystem::current_path();
    std::string buildPath;

    // get rid of executable name
    size_t last_slash_pos = resPath.find_last_of('/');
    if (last_slash_pos != std::string::npos) {
        resPath.erase(last_slash_pos);
    }

    if (resPath == cwd) {

        // clicking on exe file case
        return cwd + '/';

    } else {

        // remove first character (which is . because of running ./LearnOpenGL)
        resPath.erase(0,1);

        // remove everything after the last slash in argv[0]

        buildPath = cwd + resPath + '/';
        //std::cout << "cwd: " << cwd << '\n';
        //std::cout << "resPath: " << resPath << '\n';
        //std::cout << "buildPath: " << buildPath << '\n';
    }

    return buildPath;
}

void getAsteroidTranslations(glm::mat4 asteroidTranslations[ASTEROID_AMOUNT]) {

    srand(glfwGetTime());
    float radius = 30.0f;
    float offset = 20.0f;

    for (unsigned int i = 0; i < ASTEROID_AMOUNT; i++) {

        glm::mat4 model = glm::mat4(1.0f);
        float angle = (float)i / (float)ASTEROID_AMOUNT * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        float scale = (rand() % 20) / 100.0f + 0.05f;
        model = glm::scale(model, glm::vec3(scale));

        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        asteroidTranslations[i] = model;
    }
}
