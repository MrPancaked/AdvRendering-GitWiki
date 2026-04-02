//
// Created by Rene on 30-3-2026.
//

#ifndef RAWENGINE_PARTICLEMANAGER_H
#define RAWENGINE_PARTICLEMANAGER_H
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <random>

namespace core {
    class ParticleManager {
    public:
        float timeStep = 1.0f;
        float gravity = 0.0f;
        float smoothingRadius = 10.0f;
        float targetDensity = 0.01f;
        float pressureMultiplier = 0.001f;

        int particleAmount;

        float horizontalBoundary;
        float verticalBoundary;

        std::random_device rd;

        std::vector<glm::vec2> positions;
        std::vector<glm::vec2> velocities;
        std::vector<float> densities;

        ParticleManager(const int particleAmount, const int& screenWidth, const int& screenHeight) : particleAmount(particleAmount), horizontalBoundary(static_cast<float>(screenWidth)), verticalBoundary(static_cast<float>(screenHeight)) {
            for (int i = 0; i < particleAmount; i++) {

                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<> widthDist(0.0f, horizontalBoundary);
                std::uniform_real_distribution<> heightDist(0.0f, verticalBoundary);

                positions.emplace_back(widthDist(gen), heightDist(gen));
                velocities.emplace_back(0.0f, 0.0f);
                densities.emplace_back(0.0f);
            }
        }

        void ChangeParticleAmount();
        void UpdateParticles(float& deltaTime);
        void SolveCollisions();
        float SmoothingKernel(const float& radius, const float& distance) const;
        float SmoothingKernelDerivative(const float& radius, const float& distance) const;
        float DensityToPressure(const float& density) const;
        float CalculateDensity(const glm::vec2& location) const;
        glm::vec2 CalculatePressureGradient(const glm::vec2& location) const;

    };
} // core

#endif //RAWENGINE_PARTICLEMANAGER_H