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

#define WIDTH 800
#define HEIGHT 600

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos); 
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const * path);
unsigned int loadCubemap(std::vector<std::string> faces);

std::string getBuildPath(std::string command);

// timekeeping
float deltaTime = 0.0f; // time between current and last frame
float lastFrame = 0.0f; // time from start to last frame being rendered

// camera
const glm::vec3 initCameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
const glm::vec3 initCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 initCameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

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
    glDepthFunc(GL_LESS);

    glEnable(GL_PROGRAM_POINT_SIZE);

    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    // Shader Setup
    std::string buildPath = getBuildPath(std::string (argv[0]));
    std::string shaderPath = buildPath + "shaders/";

    // back to boring setup stuff now
    Shader shader((shaderPath + "shader/shader.vert").c_str(), (shaderPath + "shader/shader.frag").c_str());
    Shader modelShader((shaderPath + "model/model.vert").c_str(), (shaderPath + "model/model.frag").c_str());
    Shader quadShader((shaderPath + "quad/quad.vert").c_str(), (shaderPath + "quad/quad.frag").c_str());
    Shader skyboxShader((shaderPath + "skybox/skybox.vert").c_str(), (shaderPath + "skybox/skybox.frag").c_str());
    Shader normalShader((shaderPath + "normal/normal.vert").c_str(), (shaderPath + "normal/normal.frag").c_str());
    Shader instancingShader((shaderPath + "instancing/instancing.vert").c_str(),
            (shaderPath + "instancing/instancing.frag").c_str());

    std::string objDirPath = buildPath + "resources/objects/";
    std::string backpack = "backpack/backpack.obj";
    std::string buddha = "buddha/buddha.obj";
    std::string bunny = "bunny/bunny.obj";
    std::string dragon = "dragon/dragon.obj";
    std::string sponza = "sponza/sponza.obj";

    Model modelObj(objDirPath + backpack);

    // obj model uniforms fuck me
    modelShader.use();
    modelShader.addGeomShader((shaderPath + "model/model.geom").c_str());
    modelShader.setInt("material.diffuse", 0);
    modelShader.setInt("material.specular", 0);
    modelShader.setFloat("material.shininess", 32.0f);
    modelShader.setInt("ourTex", 0);

    modelShader.setVec3("dirLight.direction", glm::vec3(1.0f, -1.0f, -1.0f));
    modelShader.setVec3("dirLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
    modelShader.setVec3("dirLight.diffuse", glm::vec3(0.4, 0.4f, 0.4f));
    modelShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    normalShader.addGeomShader((shaderPath + "normal/normal.geom").c_str());

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    float quadVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
        -1.0f, -1.0f, -0.5f,  0.0f, 0.0f,
        1.0f, -1.0f,  -0.5f,  1.0f, 0.0f,
        1.0f, 1.0f,  -0.5f,  1.0f, 1.0f,

        1.0f, 1.0f, -0.5f,  1.0f, 1.0f,								
        -1.0f, 1.0f, -0.5f,  0.0f, 1.0f,
        -1.0f, -1.0f,  -0.5f,  0.0f, 0.0f
    };

    float instancingVerticies[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        0.05f, -0.05f,  0.0f, 1.0f, 0.0f,   
        0.05f,  0.05f,  0.0f, 1.0f, 1.0f		    		
    };


    // -------------- //
    // BUFFER OBJECTS //
    // -------------- //

    // frame buffer and attachments
    // use textures as buffer space
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // allocate memory to texture, so no actual texture in there just empty space to write to
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // or we can uuse renderbuffer objects? you cant directly read from them
    // you can use glReadPixels but it is slow
    // renderbuffers are good for depth and stencil buffers because of the readonly
    // renderbuffer good for when not sampling
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // attach yippee
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // uniform buffer block
    unsigned int cameraMatrixBlock;
    glGenBuffers(1, &cameraMatrixBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraMatrixBlock);
    glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraMatrixBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // instancing objects
    //
    // make translations array
    glm::vec2 translations[100];

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            translations[j * 10 + i].x = -float(i) / 5.0f + 0.9f;
            translations[j * 10 + i].y = float(j) / 5.0f - 0.9f;
        }
    }

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int instancingVAO, instancingVBO;
    glGenVertexArrays(1, &instancingVAO);
    glGenBuffers(1, &instancingVBO);
    glBindVertexArray(instancingVAO);
    glBindBuffer(GL_ARRAY_BUFFER, instancingVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(instancingVerticies), &instancingVerticies, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);
    glBindVertexArray(0);

    // ------------- //
    // Load Textures //
    // ------------- //
    std::string cubeTexturePath = buildPath + "resources/textures/container.jpg";
    std::string floorTexturePath = buildPath + "resources/textures/metal.jpg";

    std::vector<std::string> faces {
        buildPath + "resources/textures/skybox/right.jpg",
            buildPath + "resources/textures/skybox/left.jpg",
            buildPath + "resources/textures/skybox/top.jpg",
            buildPath + "resources/textures/skybox/bottom.jpg",
            buildPath + "resources/textures/skybox/front.jpg",
            buildPath + "resources/textures/skybox/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);  
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    // load the camera projection matrix into the uniform buffer object memory
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov),
            (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraMatrixBlock);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &projection);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // --------- //
    // Main Loop //
    // --------- //
    while (!glfwWindowShouldClose(window)) {
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // rendering config here
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        /*
        // get camera matrices
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // load view matrix into memory
        glBindBuffer(GL_UNIFORM_BUFFER, cameraMatrixBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, &view);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        
        // render obj models
        modelShader.use();
        model = glm::mat4(1.0f);
        modelShader.setMat4("model", model);
        modelShader.setFloat("time", currentFrame);
        modelObj.Draw(modelShader);

        normalShader.use();
        normalShader.setMat4("projection", projection);
        normalShader.setMat4("view", view);
        normalShader.setMat4("model", model);
        modelObj.Draw(normalShader);

        // render skybox
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.use();
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("view", view);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        */

        // draw tha quads
        instancingShader.use();
        glBindVertexArray(instancingVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

        // do the framebuffer thing
        quadShader.use();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.4f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render quad
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // i think i need to delete all the things here
  
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
