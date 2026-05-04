#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "stb_image.h"

#include <iostream>

enum ShaderMode {
    PHONG,
    GOOCH,
    XTOON
};

ShaderMode currentMode = PHONG; //default

//Window dimensions 
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

//Camera
Camera camera(glm::vec3(0.0f, 0.5f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT/ 2.0f;
bool firstMouse = true;

//Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse){
        lastX = xpos; lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY=ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action == GLFW_PRESS){
        if(key == GLFW_KEY_1) {
            currentMode = PHONG;
            std::cout << "Mode: PHONG\n";
        }
        if(key == GLFW_KEY_2){
            currentMode = GOOCH;
            std::cout << "Mode: GOOCH\n";
        }
        if(key == GLFW_KEY_3){
            currentMode = XTOON;
            std::cout << "Mode: XTOON\n";
        }
    }
}

void processInput(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

unsigned int loadTexture(const char* path){
    unsigned int texID;
    glGenTextures(1, &texID);
    int w, h, nChannels;
    unsigned char* data = stbi_load(path, &w, &h, &nChannels, 0);
    if(data){
        GLenum fmt = (nChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h , 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else{
        std::cerr << "Failed to load texture: " << path << std::endl;
    }

    return texID;

}

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#endif
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "NPR Shaders", NULL, NULL);
    if(!window){
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);  
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Load shader and model
    Shader phongShader("../shaders/phong.vert", "../shaders/phong.frag");
    Shader goochShader("../shaders/phong.vert", "../shaders/gooch.frag");
    Shader outlineShader("../shaders/outline.vert", "../shaders/outline.frag");
    Shader xtoonShader("../shaders/phong.vert", "../shaders/xtoon.frag");
    
    unsigned int xtoonTex = loadTexture("../textures/xtoon_ramp.png");

    Model spot("../models/spot/spot_triangulated.obj");

    //Shared light position
    glm::vec3 lightPos(0.0f, 2.0f, 3.0f);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrices
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom),
            (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(180.0f),glm::vec3(0.0f, 1.0f, 0.0f));

        if(currentMode == PHONG){
            glCullFace(GL_BACK);
            phongShader.use();
            phongShader.setMat4("projection", projection);
            phongShader.setMat4("view", view);
            phongShader.setMat4("model", model);

            // Light and material uniforms
            phongShader.setVec3("lightPos", lightPos);
            phongShader.setVec3("lightColor", glm::vec3(1.0f));
            phongShader.setVec3("viewPos", camera.Position);
            phongShader.setVec3("objectColor", glm::vec3(0.7f, 0.5f, 0.3f));
            phongShader.setFloat("ambientStrength", 0.15f);
            phongShader.setFloat("specularStrength", 0.6f);
            phongShader.setFloat("shininess", 32.0f);
            phongShader.setBool("useTexture", true); 
            
            spot.Draw(phongShader);
        }
        else if(currentMode == GOOCH){

            //PASS 1 - blackoutline shell 
            glCullFace(GL_FRONT);
            outlineShader.use();
            outlineShader.setMat4("projection", projection);
            outlineShader.setMat4("view", view);
            outlineShader.setMat4("model",model);
            outlineShader.setFloat("outlineThickness", 0.03f);
            spot.Draw(outlineShader);

            //PASS 2 - Gooch shading
            glCullFace(GL_BACK);
            goochShader.use();
            goochShader.setMat4("projection", projection);
            goochShader.setMat4("view", view);
            goochShader.setMat4("model", model);
            goochShader.setVec3("lightPos", lightPos);
            goochShader.setVec3("viewPos", camera.Position);
            goochShader.setVec3("objectColor", glm::vec3(0.7f, 0.5f, 0.3f));
            goochShader.setFloat("alpha", 0.45f);
            goochShader.setFloat("beta", 0.45f);
            spot.Draw(goochShader);
        }
        else if(currentMode == XTOON){
            //pass 1 : outline
            glCullFace(GL_FRONT);
            outlineShader.use();
            outlineShader.setMat4("projection", projection);
            outlineShader.setMat4("view", view);
            outlineShader.setMat4("model", model);
            outlineShader.setFloat("outlineThickness", 0.01f);
            spot.Draw(outlineShader);

            //pass 2 - xtoon shading
            glCullFace(GL_BACK);
            xtoonShader.use();
            xtoonShader.setMat4("projection", projection);
            xtoonShader.setMat4("view",view);
            xtoonShader.setMat4("model", model);
            xtoonShader.setVec3("lightPos", lightPos);
            xtoonShader.setVec3("viewPos", camera.Position);

            //tune at camera distance ~2 full detail, ~8 fully abstracted
            xtoonShader.setFloat("nearDist", 2.0f);
            xtoonShader.setFloat("farDist", 8.0f);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, xtoonTex);
            xtoonShader.setInt("xtoonTex", 0);

            spot.Draw(xtoonShader);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}