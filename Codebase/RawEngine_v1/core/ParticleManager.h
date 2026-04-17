//
// Created by Rene on 30-3-2026.
//

#ifndef RAWENGINE_PARTICLEMANAGER_H
#define RAWENGINE_PARTICLEMANAGER_H
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <random>

#include "Shader.h"

namespace core {
    class ParticleManager {
    public:
        float gravity = 0.0f;
        float smoothingRadius = 0.25f;
        float targetDensity = 50.0f;
        float pressureMultiplier = 1.5f;
        float collisionDamping = 0.5f;
        float viscosityStrength = 1.0f;
        float boundaryForceStrength = 0.5f;
        float boundaryForceRange = 0.25f;

        int particleAmount;

        float texelDensity = 200.0f;
        float horizontalBoundary;
        float verticalBoundary;

        bool applyInputForce = false;
        float inputForceRadius = 1.0f;
        float inputForceStrength = 0.5f;

        glm::vec2 mousePos{};

        std::random_device rd;

        float mass = 1.0f; //every particle has the same mass for now
        std::vector<glm::vec2> positions;
        std::vector<glm::vec2> predictedPositions;
        std::vector<glm::vec2> velocities;
        std::vector<float> densities;

        ParticleManager(const int particleAmount, const int& screenWidth, const int& screenHeight) : particleAmount(particleAmount), horizontalBoundary(static_cast<float>(screenWidth) / texelDensity), verticalBoundary(static_cast<float>(screenHeight) / texelDensity){
            for (int i = 0; i < particleAmount; i++) {

                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<> widthDist(0.0f, horizontalBoundary);
                std::uniform_real_distribution<> heightDist(0.0f, verticalBoundary);

                positions.emplace_back(widthDist(gen), heightDist(gen));
                predictedPositions.emplace_back(0.0f);
                velocities.emplace_back(0.0f, 0.0f);
                densities.emplace_back(0.0f);
            }
        }

        void ChangeParticleAmount();
        void UpdateParticles(const float& deltaTime);
        void SolveCollisions();
        float SmoothingKernel(const float& radius, const float& distance) const;
        float SmoothingKernelDerivative(const float& radius, const float& distance) const;
        float DensityToPressure(const float& density) const;
        float CalculateDensity(const glm::vec2& location) const;
        float CalculateSharedPressure(const float& density1, const float& density2) const;
        glm::vec2 ApplyInputForce(const glm::vec2& inputPos, const int& particleIndex , const float& radius, const float& strength);
        glm::vec2 CalculateViscosityForce(const int& particleIndex);
        glm::vec2 CalculateBoundaryForces(const int& particleIndex);
        glm::vec2 CalculatePressureGradient(const int& particleIndex) const;
        void SetBoundaries(const int& screenWidth, const int& screenHeight);
        void UpdateShader(const Shader& shader) const;
    };
} // core

#endif //RAWENGINE_PARTICLEMANAGER_H