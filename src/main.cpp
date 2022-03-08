#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gli/texture2d.hpp>
#include <gli/load.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "main.h"
#include "shader.h"
#include "utils.h"
#include "camera.h"
#include "common.h"

int scrWidth = 1000, scrHeight = 1000;

float lastX = (float)scrWidth / 2.0f, lastY = (float)scrHeight / 2.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float mixValue = 0.0f;
bool firstMouse = true;

Camera primaryCamera(glm::vec3(0.0, 0.0, 3.0));

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    scrWidth = width;
    scrHeight = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    primaryCamera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    primaryCamera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixValue += 1.0f * deltaTime; // change this value accordingly (might be too slow or too fast based on system hardware)
        if(mixValue >= 5.0f) {
            mixValue = 5.0f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixValue -= 1.0f * deltaTime; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue <= -5.0f) {
            mixValue = -5.0f;
        }
    }

    const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        primaryCamera.ProcessKeyboard(ECameraMovement::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        primaryCamera.ProcessKeyboard(ECameraMovement::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        primaryCamera.ProcessKeyboard(ECameraMovement::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        primaryCamera.ProcessKeyboard(ECameraMovement::RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        primaryCamera.ProcessKeyboard(ECameraMovement::UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        primaryCamera.ProcessKeyboard(ECameraMovement::DOWN, deltaTime);
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(scrWidth, scrHeight, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    Shader lightingShader("../shaders/litModel.vert", "../shaders/litModel.frag");
//    Shader lightingSourceShader("../shaders/simple.vert", "../shaders/simpleLightSource.frag");

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    glm::vec3 pointLightPositions[] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    Model backpack("../assets/backpack/backpack.obj");

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        float lightOffset = mixValue * 3.0f;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 lightColor(1.0f);

        glm::vec3 diffuseColor = lightColor   * glm::vec3(1.0f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(1.0f);

        lightingShader.use();
        lightingShader.setVec3("viewPos", primaryCamera.GetPosition());
        lightingShader.setInt("material.diffuse", 0);
        lightingShader.setInt("material.specular", 1);
        lightingShader.setInt("material.emissive", 2);
        lightingShader.setFloat("material.shininess", 76.8f);


        lightingShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        lightingShader.setVec3("dirLight.ambient", ambientColor);
        lightingShader.setVec3("dirLight.diffuse", diffuseColor);
        lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        for (size_t i = 0; i < 4; ++i) {
            std::string prefix = "pointLights[" + std::to_string(i) + "].";
            lightingShader.setVec3(prefix + "position",  pointLightPositions[i] + glm::vec3(lightOffset, 0.0f, 0.0f));
            lightingShader.setVec3(prefix + "ambient", ambientColor);
            lightingShader.setVec3(prefix + "diffuse", diffuseColor);
            lightingShader.setVec3(prefix + "specular", 1.0f, 1.0f, 1.0f);
            lightingShader.setFloat(prefix + "constant",  1.0f);
            lightingShader.setFloat(prefix + "linear",    0.19f);
            lightingShader.setFloat(prefix + "quadratic", 0.232f);
        }

        lightingShader.setVec3("spotLight.position",  primaryCamera.GetPosition());
        lightingShader.setVec3("spotLight.direction", primaryCamera.GetFrontVector());
        lightingShader.setFloat("spotLight.cutoff",   glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutoff", glm::cos(glm::radians(16.0f)));
        lightingShader.setVec3("spotLight.ambient", ambientColor);
        lightingShader.setVec3("spotLight.diffuse", diffuseColor);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant",  1.0f);
        lightingShader.setFloat("spotLight.linear",    0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);

        glm::mat4 view;
        view = primaryCamera.GetViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(primaryCamera.GetZoom()), (float) scrWidth / (float) scrHeight, 0.1f, 100.0f);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 mvpMat = projection * view * model;
        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(model)));
        lightingShader.setMat4("mvp", mvpMat);
        lightingShader.setMat4("model", model);
        lightingShader.setMat3("normal", normalMat);
        backpack.Draw(lightingShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
