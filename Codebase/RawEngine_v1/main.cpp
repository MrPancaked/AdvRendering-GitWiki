#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iomanip>

#include "core/ComputeParticleManager.h"
#include "core/ParticleManager.h"
#include "core/ParticleQuad.h"
#include "core/Shader.h"

double accumulatedTime = 0;

int g_width = 1200;
int g_height = 800;

double xpos, ypos;

std::ofstream myfile;

int particleAmount = 9000;
core::ParticleManager particleManager(particleAmount, g_width, g_height);
core::ComputeParticleManager computeParticleManager(particleAmount, g_width, g_height);

void writeToFile(const int& frame, const double& deltaTime, const double& computeTime, const double& renderTime) {
    if (myfile.is_open()) {
        myfile << frame << "," << deltaTime << "," << computeTime << "," << renderTime << "\n";
    }
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    printf("cursor position %f, %f\n", xpos, ypos);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        particleManager.applyInputForce = 1;
        computeParticleManager.applyInputForce = 1;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        particleManager.applyInputForce = -1;
        computeParticleManager.applyInputForce = -1;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        particleManager.applyInputForce = 0;
        computeParticleManager.applyInputForce = 0;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        particleManager.applyInputForce = 0;
        computeParticleManager.applyInputForce = 0;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    g_width = width;
    g_height = height;

    particleManager.screenWidth = static_cast<float>(g_width);
    particleManager.screenHeight = static_cast<float>(g_height);
    computeParticleManager.screenWidth = static_cast<float>(g_width);
    computeParticleManager.screenHeight = static_cast<float>(g_height);

    printf("width: %d, height: %d\n", g_width, g_height);
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

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
    ImGui_ImplOpenGL3_Init("#version 430");

    int work_grp_cnt[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
    std::cout << "Max work groups per compute shader" <<
        " x:" << work_grp_cnt[0] <<
        " y:" << work_grp_cnt[1] <<
        " z:" << work_grp_cnt[2] << "\n";

    int work_grp_size[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
    std::cout << "Max work group sizes" <<
        " x:" << work_grp_size[0] <<
        " y:" << work_grp_size[1] <<
        " z:" << work_grp_size[2] << "\n";

    int work_grp_inv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";

    core::Shader computeShader("shaders/compute.comp");
    computeParticleManager.InitialiseBuffers();

    core::Shader particleQuadShader("shaders/particleVertex.vert", "shaders/particleFragment.frag");

    core::ParticleQuad particleQuad;

    glm::vec3 particleColor1 = glm::vec3(1.0f, 0.0f, 1.0f);
    glm::vec3 particleColor2 = glm::vec3(0.0f, 1.0f, 1.0f);
    glm::vec3 backgroundColor = glm::vec3(0.1f, 0.0f, 0.25f);

    float particleRadius = 10.0f;

    bool useComputeShader = true;


    int simulationSteps = 1;
    double elapsedSecs;
    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    double deltaTime = 0.001f;
    double computeTime = 0.0f;
    double renderTime = 0.0f;
    int frame = 0;

    bool record = false;
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::stringstream ss;
    ss  << "Data/GPU_"
        << std::to_string(particleAmount) << "_"
        << std::put_time(localTime, "%Y-%m-%d_%H.%M.%S")
        << ".csv";
    myfile.open(ss.str());
    myfile << "Frame,DeltaTime,ComputeTime,RenderTime\n";

    while (!glfwWindowShouldClose(window)) {
        clock_t begin = clock();

        glfwGetCursorPos(window, &xpos, &ypos);
        ypos = (ypos - g_height) * -1.0f;
        particleManager.mousePos = glm::vec2(xpos, ypos);
        computeParticleManager.mousePos = glm::vec2(xpos, ypos);

        processInput(window);
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        double timeBeforeCompute = glfwGetTime();
        for (int i = 0; i < simulationSteps; i++) {
            if (!useComputeShader) {
                // update all particles
                particleManager.UpdateParticles(deltaTime);
            }
            else {
                computeShader.use();
                computeShader.setVec2("boundaries", glm::vec2(computeParticleManager.horizontalBoundary, computeParticleManager.verticalBoundary));
                computeShader.setFloat("collisionDamping", computeParticleManager.collisionDamping);
                computeShader.setFloat("gravity", computeParticleManager.gravity);
                computeShader.setFloat("mass", computeParticleManager.mass);
                computeShader.setFloat("smoothingRadius", computeParticleManager.smoothingRadius);
                computeShader.setFloat("pressureMultiplier", computeParticleManager.pressureMultiplier);
                computeShader.setFloat("targetDensity", computeParticleManager.targetDensity);
                computeShader.setVec2("mousePos", computeParticleManager.mousePos);
                computeShader.setInt("applyInputForce", computeParticleManager.applyInputForce);
                computeShader.setFloat("inputForceRadius", computeParticleManager.inputForceRadius);
                computeShader.setFloat("inputForceStrength", computeParticleManager.inputForceStrength);
                computeShader.setFloat("texelDensity", computeParticleManager.texelDensity);
                computeShader.setFloat("deltaTime", deltaTime);
                computeParticleManager.UpdateParticles(computeShader);
            }
        }
        double timeAfterCompute = glfwGetTime();
        computeTime = timeAfterCompute - timeBeforeCompute;

        particleQuadShader.use(); //switch from compute shader to normal shader
        if (!useComputeShader) {
            particleManager.calculateScreenSpacePos();
            // updating shader with particle information and RENDERING PARTICLES
            for (int i = 0; i < particleManager.particleAmount; i++) {
                particleQuadShader.setVec2("particlePos", particleManager.scrSpacePositions[i]);
                particleQuadShader.setVec2("velocity", particleManager.velocities[i]);
                particleQuad.RenderQuad();
            }
        }
        else {
            computeParticleManager.calculateScreenSpacePos();
            // updating shader with particle information and RENDERING PARTICLES
            for (int i = 0; i < computeParticleManager.particleAmount; i++) {
                particleQuadShader.setVec2("particlePos", computeParticleManager.scrSpacePositions[i]);
                particleQuadShader.setVec2("velocity", computeParticleManager.velocities[i]);
                particleQuad.RenderQuad();
            }
        }
        renderTime = glfwGetTime() - timeAfterCompute;

        // do everything ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Information");

        ImGui::Text("Screen Size: %d, %d", g_width, g_height);
        if (computeParticleManager.particleAmount > 0) {
            glm::vec2 position = computeParticleManager.positions[0];
            ImGui::Text("position at particle1 = %f, %f\n", position.x, position.y);
        }
        ImGui::Text("Total run time: %f\n", accumulatedTime);
        ImGui::Text("delta time = %f\n", deltaTime);
        ImGui::End();

        ImGui::Begin("Settings");
        if (ImGui::TreeNode("General Settings")) {
            ImGui::Checkbox("Record DeltaTime", &record);
            ImGui::Checkbox("Use Compute Shader", &useComputeShader);
            ImGui::SliderInt("Simulation Steps", &simulationSteps, 0, 10);
            if (ImGui::Button("Next Frame")) {
                if (!useComputeShader) {
                    // update all particles
                    particleManager.UpdateParticles(deltaTime);
                }
                else {
                    computeParticleManager.UpdateParticles(computeShader);
                }
            }
            ImGui::ColorEdit3("Background Color", glm::value_ptr(backgroundColor));
            ImGui::ColorEdit3("Color1", glm::value_ptr(particleColor1));
            ImGui::ColorEdit3("Color2", glm::value_ptr(particleColor2));
            ImGui::SliderFloat("Visual Radius (in pixels)", &particleRadius, 1.0f, 100.0f);

            ImGui::TreePop();
            ImGui::Separator();
        }
        if (ImGui::TreeNode("CPU Particle Settings")) {
            ImGui::SliderInt("Amount", &particleManager.particleAmount, 0, 1000);
            ImGui::DragFloat("Gravity", &particleManager.gravity, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Mass", &particleManager.mass, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Collision Damping", &particleManager.collisionDamping, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Input force strength", &particleManager.inputForceStrength, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Boundary Force Strength", &particleManager.boundaryForceStrength, 0.01f, 0.0f, 100.0f);
            ImGui::DragFloat("PressureMultiplier", &particleManager.pressureMultiplier, 0.001f, 0.0f, 100.0f);
            ImGui::DragFloat("Target Density", &particleManager.targetDensity, 0.01f, 0.0f, 200.0f);
            ImGui::SliderFloat("Smoothing Radius (in units)", &particleManager.smoothingRadius, 0.0f, 1.0f);
            ImGui::SliderFloat("Texel Density (pixels / unit)", &particleManager.texelDensity, 1.0f, 1000.0f);

            ImGui::TreePop();
        }
        if (ImGui::TreeNode("GPU Particle Settings")) {
            ImGui::SliderInt("Amount", &computeParticleManager.particleAmount, 0, 10000);
            ImGui::DragFloat("Gravity", &computeParticleManager.gravity, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Mass", &computeParticleManager.mass, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Collision Damping", &computeParticleManager.collisionDamping, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Input force strength", &computeParticleManager.inputForceStrength, 0.001f, 0.0f, 1.0f);
            ImGui::DragFloat("Boundary Force Strength", &computeParticleManager.boundaryForceStrength, 0.01f, 0.0f, 100.0f);
            ImGui::DragFloat("PressureMultiplier", &computeParticleManager.pressureMultiplier, 0.001f, 0.0f, 100.0f);
            ImGui::DragFloat("Target Density", &computeParticleManager.targetDensity, 0.01f, 0.0f, 200.0f);
            ImGui::SliderFloat("Smoothing Radius (in units)", &computeParticleManager.smoothingRadius, 0.0f, 1.0f);
            ImGui::SliderFloat("Texel Density (pixels / unit)", &computeParticleManager.texelDensity, 1.0f, 1000.0f);

            ImGui::TreePop();
        }
        ImGui::End();

        particleQuadShader.use();
        particleQuadShader.setVec2("screenSize", glm::vec2(static_cast<float>(g_width), static_cast<float>(g_height)));
        particleQuadShader.setFloat("particleRad", particleRadius);
        particleQuadShader.setVec3("particleColor1", particleColor1);
        particleQuadShader.setVec3("particleColor2", particleColor2);

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

        if (frame >= 1000) {
            record = false;
        }
        if (record) {
            writeToFile(frame, deltaTime, computeTime, renderTime);
            frame++;
        }

    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(particleQuadShader.ID);
    glfwTerminate();

    myfile.close();
    return 0;
}