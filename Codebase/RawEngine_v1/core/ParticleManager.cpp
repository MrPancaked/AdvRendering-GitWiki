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
        if (particleAmount > positions.size()) {
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> widthDist(0.0f, horizontalBoundary);
            std::uniform_real_distribution<> heightDist(0.0f, verticalBoundary);

            int increaseAmount =  particleAmount - positions.size();
            for (int i = 0; i < increaseAmount; i++) {
                positions.emplace_back(widthDist(gen), heightDist(gen));
                velocities.emplace_back(0.0f);
                densities.emplace_back(0.0f);
            }
        }
        else if (particleAmount < positions.size()) {
            positions.resize(particleAmount);
            velocities.resize(particleAmount);
            densities.resize(particleAmount);
        }
    }

    void ParticleManager::UpdateParticles(float& deltaTime) {

        // calling this every frame is okay since std::vector<>.resize only does anything when size actually changes.
        ChangeParticleAmount();

        for (int i = 0; i < particleAmount; i++) {
            densities[i] = CalculateDensity(positions[i]);
        }

        for (int i = 0; i < particleAmount; i++) {
            glm::vec2 gravityComp = glm::vec2(0.0f, -1.0f) * gravity  * deltaTime;
            glm::vec2 pressureComp = CalculatePressureGradient(positions[i]);

            velocities[i] += gravityComp + pressureComp * pressureMultiplier;
            positions[i] += velocities[i] * timeStep;
            //printf("pressureComp%d: %f, %f\n", i, pressureComp.x, pressureComp.y);
            //printf("velocity%d: %f, %f\n", i, velocities[i].x, velocities[i].y);
        }
        SolveCollisions();
    }

    void ParticleManager::SolveCollisions() {
        for (int i = 0; i < particleAmount; i++) {
            glm::vec2& position = positions[i];
            glm::vec2& velocity = velocities[i];
            if (position.x < 0) {
                position.x = 0;
                velocity.x = -velocity.x;
            }
            else if (position.x > horizontalBoundary) {
                position.x = horizontalBoundary;
                velocity.x = -velocity.x;
            }
            if (position.y < 0) {
                position.y = 0;
                velocity.y = -velocity.y;
            }
            else if (position.y > verticalBoundary) {
                position.y = verticalBoundary;
                velocity.y = -velocity.y;
            }
        }
    }
    float ParticleManager::SmoothingKernel(const float& radius, const float& distance) const{

        if (distance > radius) {return 0.0f;}
        //volume of smoothing kernel
        float volume = 2.0f * glm::pi<float>() * glm::pow(smoothingRadius, 3.0f) / 3.0f;
        return (radius - distance) * (radius - distance) / volume;
    }
    float ParticleManager::SmoothingKernelDerivative(const float& radius, const float& distance) const{
        if (distance > radius) {return 0.0f;}
        return (2.0f * distance - 2.0f * radius);
        //return (4.0f * glm::pi<float>() * glm::pow(smoothingRadius, 3.0f) / 3.0f) * (distance - radius);
        //return ((4.0f * glm::pi<float>() * glm::pow(smoothingRadius, 3.0f) * distance) / 3.0f) - ((4.0f * glm::pi<float>() * glm::pow(smoothingRadius, 4.0f)) / 3.0f);
    }

    float ParticleManager::CalculateDensity(const glm::vec2& location) const {
        float density = 0.0f;
        float mass = 1.0f;

        for (auto position : positions) {
            float influence = SmoothingKernel(smoothingRadius, glm::distance(position, location));
            density += influence * mass;
        }
        return density;
    }

    float ParticleManager::DensityToPressure(const float& density) const {
        float densityDelta = density - targetDensity;
        return densityDelta;
    }

    glm::vec2 ParticleManager::CalculatePressureGradient(const glm::vec2& location) const {
        glm::vec2 pressureGradient = glm::vec2(0.0f, 0.0f);

        for (int i = 0; i < particleAmount; i++) {

            float distance = glm::distance(positions[i], location);
            //skip over itself
            if (distance < 1e-6f) {continue;} // this might cause particles to lose energy but it almost never happens so who cares
            glm::vec2 direction = (positions[i] - location) / distance;
            float slope = SmoothingKernelDerivative(smoothingRadius, distance);
            float density = densities[i];
            pressureGradient += DensityToPressure(density) * direction * -slope * 1.0f / density; //1.0f is the mass of the particle
            //printf("slope at distance%f: %f\n", distance, slope);
        }
        return pressureGradient;
    }
} // core