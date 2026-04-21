//
// Created by Rene on 20-4-2026.
//

#ifndef RAWENGINE_COMPUTEPARTICLEMANAGER_H
#define RAWENGINE_COMPUTEPARTICLEMANAGER_H
#include <glad/glad.h>
#include <vector>
#include <glm/vec2.hpp>
#include <random>

namespace core {
    class ComputeParticleManager {
    public:
        int particleAmount;
        std::vector<glm::vec2> positions;
        std::vector<glm::vec2> predictedPositions;
        std::vector<glm::vec2> scrSpacePositions;
        std::vector<glm::vec2> velocities;
        std::vector<float> densities;

        float mass = 1.0f;
        float gravity = 0.2f;
        float smoothingRadius = 0.25f;
        float targetDensity = 100.0f;
        float pressureMultiplier = 3.0f;
        float collisionDamping = 0.5f;
        float viscosityStrength = 1.0f;
        float boundaryForceStrength = 0.0f;
        float boundaryForceRange = 0.25f;

        float texelDensity = 200.0f;
        float screenWidth;
        float screenHeight;
        float horizontalBoundary;
        float verticalBoundary;

        int applyInputForce = 0;
        float inputForceRadius = 150.0f;
        float inputForceStrength = 0.5f;

        glm::vec2 mousePos{};

        std::random_device rd;

        GLuint positionBuffer{};
        GLuint velocityBuffer{};
        GLuint predictedPositionBuffer{};
        GLuint densityBuffer{};

        GLuint particleVAO{};

        ComputeParticleManager(const int particleAmount, const int& screenWidth, const int& screenHeight) : particleAmount(particleAmount), screenWidth(static_cast<float>(screenWidth)), screenHeight(static_cast<float>(screenHeight)){

            SetBoundaries();
            for (int i = 0; i < particleAmount; i++) {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<> widthDist(0.0f, horizontalBoundary);
                std::uniform_real_distribution<> heightDist(0.0f, verticalBoundary);

                positions.emplace_back(widthDist(gen), heightDist(gen));
                scrSpacePositions.emplace_back(0.0f);
                predictedPositions.emplace_back(0.0f);
                densities.emplace_back(0.0f);
                velocities.emplace_back(0.0f, 0.0f);
            }
            calculateScreenSpacePos();
            //InitialiseBuffers(); // THIS DOESN'T WORK BECAUSE I AM INITIALISING IT BEFORE Main() (before glad and everything important)
        }
        void InitialiseBuffers();
        void ChangeParticleAmount();
        void calculateScreenSpacePos();
        void SetBoundaries();
    private:

    };
} // core

#endif //RAWENGINE_COMPUTEPARTICLEMANAGER_H