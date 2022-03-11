#include <iostream>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "main.h"
#include "shader.h"
#include "camera.h"
#include "common.h"

int scrWidth = 1920, scrHeight = 1080;

float lastX = (float)scrWidth / 2.0f, lastY = (float)scrHeight / 2.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float mixValue = 0.0f;
bool firstMouse = true;

std::map<char, Character> Characters;
GLuint fontBrushVAO, fontBrushVBO;

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

class LightingSetupHelper{
public:
    LightingSetupHelper(): lightColor(1.0f) {
        diffuseColor = lightColor   * glm::vec3(1.0f);
        ambientColor = diffuseColor * glm::vec3(1.0f);
    }
    void UpdateShader(Shader& lightingShader) {
        float lightOffset = mixValue * 3.0f; // for demo purposes

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
    }
private:
    glm::vec3 pointLightPositions[4] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    glm::vec3 lightColor, diffuseColor, ambientColor;
};

class ModelSetupHelper{
public:
    ModelSetupHelper() : scale(glm::vec3(1.0f)), rotationAxis(glm::vec3(1.0f)), location(glm::vec3(0.0f)), rotationAngleRadians(0.0f) {}
    void SetScale(glm::vec3 scale) {
        this->scale = scale;
    }
    void SetRotation(glm::vec3 rotationAxis, float angleDegrees) {
        this->rotationAxis = rotationAxis;
        this->rotationAngleRadians = glm::radians(angleDegrees);
    }
    void SetLocation(glm::vec3 location) {
        this->location = location;
    }
    void UpdateShader(Shader& shader) {
        view = primaryCamera.GetViewMatrix();
        projection = glm::perspective(glm::radians(primaryCamera.GetZoom()), (float) scrWidth / (float) scrHeight, 0.1f, 100.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, location);
        model = glm::rotate(model, rotationAngleRadians, rotationAxis);
        model = glm::scale(model, scale);
        glm::mat4 mvpMat = projection * view * model;
        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(model)));
        shader.setMat4("mvp", mvpMat);
        shader.setMat4("model", model);
        shader.setMat3("normal", normalMat);
    }
private:
    glm::mat4 view, projection;
    glm::vec3 scale, rotationAxis, location;
    float rotationAngleRadians;
};

GLuint setupScreenVAO() {
    float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    return quadVAO;
}

// returns (VAO, VBO)
std::tuple<GLuint, GLuint> prepareFontBrush() {
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW); // 6 vertices, 4 floats each
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return {VAO, VBO};
}

void prepareFonts() {
    auto brushInfo = prepareFontBrush();
    fontBrushVAO = get<0>(brushInfo);
    fontBrushVBO = get<1>(brushInfo);
    auto maybeChars = loadFonts("../../fonts/FreeMono.ttf", 25);
    if (maybeChars.has_value()) {
        Characters = maybeChars.value();
    } else {
        std::cout << "Loading characters failed" << std::endl;
    }
}

void RenderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state
    s.use();
    s.setVec3("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(fontBrushVAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); ++c)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, fontBrushVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// returns (framebuffer, rendertexture)
std::tuple<GLuint, GLuint> setupOffscreenRenderTexture() {
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scrWidth, scrHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scrWidth, scrHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return {fbo, texture};
}

void printFrameInfo(Shader& fontShader) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream msLine;
    msLine << "frame time (ms): " << deltaTime * 1000.f;
    RenderText(fontShader, msLine.str(), 15.0f, (float)scrHeight - 35.0f, 1.0f, glm::vec3(1.0, 1.0, 1.0));
    std::stringstream fpsLine;
    fpsLine << "fps: " << 1.f / deltaTime;
    RenderText(fontShader, fpsLine.str(), 15.0f, (float)scrHeight - 60.0f, 1.0f, glm::vec3(1.0, 1.0, 1.0));
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


    Shader lightingShader("../../shaders/litModel.vert", "../../shaders/litModel.frag");
//    Shader lightingSourceShader("../../shaders/simple.vert", "../../shaders/simpleLightSource.frag");
    Shader outlineColorShader("../../shaders/litModel.vert", "../../shaders/outlineColor.frag");
    Shader screenShader("../../shaders/screen.vert", "../../shaders/screen.frag");

    Shader fontShader("../../shaders/text.vert", "../../shaders/text.frag");
    fontShader.use();
    glm::mat4 textProjection = glm::ortho(0.0f, (float)scrWidth, 0.0f, (float)scrHeight);
    fontShader.setMat4("projection", textProjection);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);
//    glDepthMask(GL_FALSE);
//    glDepthFunc(GL_ALWAYS);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    LightingSetupHelper lsh;
    ModelSetupHelper msh;

    Model backpack("../../assets/backpack/backpack.obj");
    Model grass("../../assets/grass/grass.obj", true);
    Model sponza("../../assets/crytek_sponza/sponza.obj", true);

    auto [fbo, renderTexture] = setupOffscreenRenderTexture();

    GLuint quadVAO = setupScreenVAO();
    prepareFonts();

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // FIRST PASS: Scene to Texture
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        lightingShader.use();
        lsh.UpdateShader(lightingShader);

        msh.SetLocation(glm::vec3(0.0, -1.0, 3.0));
        msh.SetRotation(glm::vec3(0.0, 1.0, 0.0), 0.0);
        msh.SetScale(glm::vec3(1.0, 1.0, 1.0));
        msh.UpdateShader(lightingShader);
        sponza.Draw(lightingShader);

        glDisable(GL_CULL_FACE);
        msh.SetLocation(glm::vec3(2.0, -1.0, 2.5));
        msh.UpdateShader(lightingShader);
        grass.Draw(lightingShader);
        glEnable(GL_CULL_FACE);


        msh.SetLocation(glm::vec3(-5.0, -0.3, 2.5));
        msh.SetRotation(glm::vec3(0.0, 1.0, 0.0), 90.0);
        msh.SetScale(glm::vec3(0.4, 0.4, 0.4));
        msh.UpdateShader(lightingShader);
        backpack.Draw(lightingShader);


        // SECOND PASS: Texture to Screen
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, renderTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // THIRD PASS: UI
        printFrameInfo(fontShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteFramebuffers(1, &fbo);

    glfwTerminate();
    return 0;
}
