#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "core/Particle.h"
#include "core/ParticleManager.h"
#include "core/Shader.h"

double accumulatedTime = 0;

int g_width = 800;
int g_height = 600;

int particleAmount = 10;
core::ParticleManager particleManager(particleAmount, g_width, g_height);

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    g_width = width;
    g_height = height;

    particleManager.horizontalBoundary = static_cast<float>(g_width);
    particleManager.verticalBoundary = static_cast<float>(g_height);

    printf("width: %d, height: %d\n", g_width, g_height);
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(g_width, g_height, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //Setup platforms
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");


    core::Shader shader("shaders/vertex.vert", "shaders/fragment.frag");

    float screenQuadVertices[] = {
        // positions   // texCoords
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,

        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW
    );

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // texcoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))
    );

    glBindVertexArray(0);

    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(clearColor.r,
                 clearColor.g, clearColor.b, clearColor.a);

    shader.use();

    glm::vec3 particleColor = glm::vec3(0.0f, 1.0f, 1.0f);
    glm::vec3 backgroundColor = glm::vec3(0.0f, 0.0f, 0.0f);

    float particleRadius = 10.0f;


    shader.setInt("particleAmount", particleAmount);



    double elapsedSecs;
    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    float deltaTime = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        clock_t begin = clock();
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.setInt("screen.width", g_width);
        shader.setInt("screen.height", g_height);

        // update all particles
        particleManager.UpdateParticles(deltaTime);

        // updating shader with particle information
        shader.setInt("particleAmount", particleManager.particleAmount);
        shader.setVec3("particleColor", particleColor);
        shader.setVec3("backgroundColor", backgroundColor);
        shader.setFloat("particleRadius", particleRadius);
        for (int i = 0; i < particleManager.particleAmount; i++) {
            std::string index = "particles[" + std::to_string(i) + "].";
            shader.setVec2(index + "position", particleManager.positions[i]);
        }

        // do everything ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Information");

        ImGui::Text("Screen Size: %d, %d", g_width, g_height);
        if (particleManager.particleAmount > 0) {
            float density = particleManager.CalculateDensity(particleManager.positions[0]);
            ImGui::Text("density at particle1 = %f\n", density);
        }
        ImGui::Text("delta time = %f\n", deltaTime);
        ImGui::End();

        ImGui::Begin("Settings");
        if (ImGui::TreeNode("General Settings")) {
            ImGui::ColorEdit3("Background Color", glm::value_ptr(backgroundColor));
            ImGui::DragFloat("Time Step", &particleManager.timeStep, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Gravity", &particleManager.gravity, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("PressureMultiplier", &particleManager.pressureMultiplier, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Target Density", &particleManager.targetDensity, 0.01f, 0.0f, 1.0f);

            ImGui::TreePop();
            ImGui::Separator();
        }
        if (ImGui::TreeNode("Particle Settings")) {
            ImGui::SliderInt("Amount", &particleManager.particleAmount, 0, 1000);
            ImGui::ColorEdit3("Color", glm::value_ptr(particleColor));
            ImGui::SliderFloat("Visual Radius", &particleRadius, 1.0f, 100.0f);
            ImGui::SliderFloat("Smoothing Radius", &particleManager.smoothingRadius, 0.0f, 1000.0f);

            ImGui::TreePop();
        }
        ImGui::End();

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;

        clock_t end = clock();
        elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
        accumulatedTime += elapsedSecs;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteProgram(shader.ID);
    glfwTerminate();
    return 0;
}