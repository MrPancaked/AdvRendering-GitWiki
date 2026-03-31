//
// Created by Rene on 30-3-2026.
//

#include "ParticleManager.h"

#include <glm/detail/func_geometric.inl>
#include <glm/ext/scalar_constants.hpp>

namespace core {
    void ParticleManager::ChangeParticleAmount() {
        if (particleAmount == positions.size()) {
            return;
        }
        if (particleAmount >= positions.size()) {
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> widthDist(0.0f, horizontalBoundary);
            std::uniform_real_distribution<> heightDist(0.0f, verticalBoundary);

            for (int i = 0; i < particleAmount - positions.size(); i++) {
                positions.emplace_back(widthDist(gen), heightDist(gen));
                velocities.emplace_back(0.0f);
            }
        }
        else if (particleAmount <= positions.size()) {
            positions.resize(particleAmount);
            velocities.resize(particleAmount);
        }
    }

    void ParticleManager::UpdateParticles(float& deltaTime) {
        for (int i = 0; i < particleAmount; i++) {
            ChangeParticleAmount(); // calling this every frame is okay since std::vector<>.resize only does anything when size actually changes.
            velocities[i] += glm::vec2(0.0f, -1.0f) * gravity  * deltaTime;
            positions[i] += velocities[i] * timeStep;
        }
    }

    float ParticleManager::SmoothingKernel(const float& radius, const float& distance) {
        if (distance > radius) {return 0.0f;}
        return (radius - distance) * (radius - distance);
    }

    float ParticleManager::CalculateDensity(const glm::vec2& location) {
        float density = 0.0f;
        float mass = 1.0f;

        //volume of smoothing kernel
        float volume = 2.0f * glm::pi<float>() * glm::pow(smoothingRadius, 3.0f) / 3.0f;

        for (auto position : positions) {

            float influence = SmoothingKernel(smoothingRadius, glm::distance(position, location));
            density += influence * mass;
        }
        return density / volume;
    }
} // core