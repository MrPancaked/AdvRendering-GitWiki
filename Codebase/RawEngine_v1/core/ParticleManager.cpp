//
// Created by Rene on 30-3-2026.
//

#include "ParticleManager.h"

#include <glm/detail/func_geometric.inl>
//#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/scalar_constants.hpp>

namespace core {


    void ParticleManager::UpdateParticles(const float& deltaTime) {
        // calling this every frame is okay since std::vector<>.resize only does anything when size actually changes.
        ChangeParticleAmount();

        //predict positions
        for (int i = 0; i < particleAmount; i++) {
            predictedPositions[i] = positions[i] + velocities[i] * 1.0f / 120.0f;
        }
        //update densities
        for (int i = 0; i < particleAmount; i++) {
            densities[i] = CalculateDensity(predictedPositions[i]);
        }
        //add gravity
        glm::vec2 gravityComp = glm::vec2(0.0f, -1.0f) * gravity;

        //final calculations
        for (int i = 0; i < particleAmount; i++) {

            glm::vec2 pressure = CalculatePressureGradient(i);
            glm::vec2 pressureComp = pressure * pressureMultiplier / densities[i];

            glm::vec2 inputForceComp = glm::vec2(0.0f);
            if (applyInputForce) inputForceComp = ApplyInputForce(mousePos, i, inputForceRadius, inputForceStrength);

            glm::vec2 boundaryForceComp = CalculateBoundaryForces(i) * pressureMultiplier / densities[i];

            velocities[i] +=  (pressureComp + gravityComp + inputForceComp + boundaryForceComp);
            positions[i] += velocities[i] * deltaTime;
            //printf("pressureComp%d: %f, %f\n", i, pressureComp.x, pressureComp.y);
            //printf("velocity%d: %f, %f\n", i, velocities[i].x, velocities[i].y);
        }
        SolveCollisions();
    }

    void ParticleManager::ChangeParticleAmount() {
        if (particleAmount == positions.size()) {
            return;
        }
        if (particleAmount > positions.size()) {
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> widthDist(0.0f, horizontalBoundary - boundaryForceRange);
            std::uniform_real_distribution<> heightDist(0.0f, verticalBoundary - boundaryForceRange);

            int increaseAmount =  particleAmount - positions.size();
            for (int i = 0; i < increaseAmount; i++) {
                positions.emplace_back(widthDist(gen), heightDist(gen));
                predictedPositions.emplace_back(0.0f);
                velocities.emplace_back(0.0f);
                densities.emplace_back(0.0f);
            }
        }
        else if (particleAmount < positions.size()) {
            positions.resize(particleAmount);
            predictedPositions.resize(particleAmount);
            velocities.resize(particleAmount);
            densities.resize(particleAmount);
        }
    }

    void ParticleManager::SolveCollisions() {
        for (int i = 0; i < particleAmount; i++) {
            glm::vec2& position = positions[i];
            glm::vec2& velocity = velocities[i];
            if (position.x < 0) {
                position.x = 0;
                velocity.x *= -1 * collisionDamping;
            }
            else if (position.x > horizontalBoundary) {
                position.x = horizontalBoundary;
                velocity.x *= -1 * collisionDamping;
            }
            if (position.y < 0) {
                position.y = 0;
                velocity.y *= -1 * collisionDamping;
            }
            else if (position.y > verticalBoundary) {
                position.y = verticalBoundary;
                velocity.y *= -1 * collisionDamping;
            }
        }
    }
    float ParticleManager::SmoothingKernel(const float& radius, const float& distance) const{

        if (distance > radius) {return 0.0f;}
        //volume of smoothing kernel
        float volume = glm::pi<float>() * glm::pow(radius, 4.0f) / 12.0f;
        return (radius - distance) * (radius - distance) / volume;
    }
    float ParticleManager::SmoothingKernelDerivative(const float& radius, const float& distance) const{
        if (distance > radius) {return 0.0f;}
        //return (2.0f * distance - 2.0f * radius);
        return -12.0f * (radius-distance) / (glm::pi<float>() * glm::pow(radius, 4.0f));

        //return (4.0f * glm::pi<float>() * glm::pow(smoothingRadius, 3.0f) / 3.0f) * (distance - radius);
        //return ((4.0f * glm::pi<float>() * glm::pow(smoothingRadius, 3.0f) * distance) / 3.0f) - ((4.0f * glm::pi<float>() * glm::pow(smoothingRadius, 4.0f)) / 3.0f);
    }

    float ParticleManager::CalculateDensity(const glm::vec2& location) const {
        float density = 0.0f;

        for (auto predictedPos : predictedPositions) {
            float influence = SmoothingKernel(smoothingRadius, glm::distance(predictedPos, location));
            density += influence * mass;
        }
        return density;
    }

    float ParticleManager::DensityToPressure(const float& density) const {
        float densityDelta = density - targetDensity;
        return densityDelta;
    }

    float ParticleManager::CalculateSharedPressure(const float& density1, const float& density2) const {
        float pressure1 = DensityToPressure(density1);
        float pressure2 = DensityToPressure(density2);
        return (pressure1 + pressure2) / 2;
    }

    glm::vec2 ParticleManager::CalculatePressureGradient(const int& particleIndex) const {
        glm::vec2 pressureGradient = glm::vec2(0.0f, 0.0f);

        for (int i = 0; i < particleAmount; i++) {
            //skip over itself
            if (particleIndex == i) {continue;}
            float distance = glm::distance(predictedPositions[i], predictedPositions[particleIndex]);
            glm::vec2 direction = glm::vec2(0.0f);
            if (distance == 0.0f) {direction = glm::normalize(predictedPositions[i] - positions[particleIndex]);}
            else {direction = (predictedPositions[i] - predictedPositions[particleIndex]) / distance;}
            float slope = SmoothingKernelDerivative(smoothingRadius, distance);
            float density = densities[i];
            float sharedPressure = CalculateSharedPressure(density, densities[particleIndex]);

            pressureGradient += sharedPressure * direction * slope * mass / density;
            // pressureGradient += direction * -slope * 1.0f / density; //1.0f is the mass of the particle
            //printf("slope at distance%f: %f\n", distance, slope);
        }
        return pressureGradient;
    }

    glm::vec2 ParticleManager::ApplyInputForce(const glm::vec2& inputPos, const int& particleIndex , const float& radius, const float& strength) {
        glm::vec2 force = glm::vec2(0.0f);
        glm::vec2 offset = inputPos - positions[particleIndex];
        float distance = glm::length(offset);
        glm::vec2 direction = glm::vec2(0.0f);
        if (distance > 0.0f)
            direction = offset / distance;
        else
            direction = glm::vec2(0.0f);
        if (distance < radius) {
            float centreForce = 1 - distance / radius;
            force = direction * centreForce * strength - 0.01f * velocities[particleIndex];
            //force = (direction * strength) * centreForce;
        }

        return force;
    }

    glm::vec2 ParticleManager::CalculateViscosityForce(const int& particleIndex) {
        glm::vec2 force = glm::vec2(0.0f);
        glm::vec2 position = positions[particleIndex];

        return force;
    }

    glm::vec2 ParticleManager::CalculateBoundaryForces(const int& particleIndex) {

        glm::vec2 direction = glm::vec2(0.0f);
        float distance = 0.0f;
        glm::vec2 force = glm::vec2(0.0f);

        if (positions[particleIndex].x <= boundaryForceRange) {
            distance = positions[particleIndex].x;
            direction = glm::vec2(1.0f, 0.0f);
            float slope = -SmoothingKernelDerivative(boundaryForceRange, distance);
            float density = densities[particleIndex];
            force += direction * boundaryForceStrength * DensityToPressure(density) * slope * mass / density;
        }
        else if (positions[particleIndex].x >= horizontalBoundary - boundaryForceRange) {
            distance = horizontalBoundary - positions[particleIndex].x;
            direction = glm::vec2(-1.0f, 0.0f);
            float slope = -SmoothingKernelDerivative(boundaryForceRange, distance);
            float density = densities[particleIndex];
            force += direction * boundaryForceStrength * DensityToPressure(density) * slope * mass / density;
        }
        if (positions[particleIndex].y <= boundaryForceRange) {
            distance = positions[particleIndex].y;
            direction = glm::vec2(0.0f, 1.0f);
            float slope = -SmoothingKernelDerivative(boundaryForceRange, distance);
            float density = densities[particleIndex];
            force += direction * boundaryForceStrength * DensityToPressure(density) * slope * mass / density;
        }
        else if (positions[particleIndex].y >= verticalBoundary - boundaryForceRange) {
            distance = verticalBoundary - positions[particleIndex].y ;
            direction = glm::vec2(0.0f, -1.0f);
            float slope = -SmoothingKernelDerivative(boundaryForceRange, distance);
            float density = densities[particleIndex];
            force += direction * boundaryForceStrength * DensityToPressure(density) * slope * mass / density;
        }
        return force;
    }
} // core