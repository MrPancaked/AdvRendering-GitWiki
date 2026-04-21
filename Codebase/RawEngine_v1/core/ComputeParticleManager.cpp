//
// Created by Rene on 20-4-2026.
//

#include "ComputeParticleManager.h"



namespace core {
    void ComputeParticleManager::InitialiseBuffers() {
        glGenBuffers(1, &positionBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
        GLuint bufferSize = positions.size() * sizeof(glm::vec2);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, positions.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &velocityBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
        bufferSize = velocities.size() * sizeof(glm::vec2);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, velocities.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &predictedPositionBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, predictedPositionBuffer);
        bufferSize = predictedPositions.size() * sizeof(glm::vec2);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, predictedPositions.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, predictedPositionBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &densityBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, densityBuffer);
        bufferSize = densities.size() * sizeof(float);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, densities.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, densityBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void ComputeParticleManager::ChangeParticleAmount() {
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
                predictedPositions.emplace_back(0.0f);
                scrSpacePositions.emplace_back(0.0f);
                densities.emplace_back(0.0f);
                velocities.emplace_back(0.0f);
            }
        }
        else if (particleAmount < positions.size()) {
            positions.resize(particleAmount);
            scrSpacePositions.resize(particleAmount);
            predictedPositions.resize(particleAmount);
            densities.resize(particleAmount);
            velocities.resize(particleAmount);
        }

        //resize SSBO
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER,positions.size() * sizeof(glm::vec2),positions.data(),GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER,velocities.size() * sizeof(glm::vec2),velocities.data(),GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, densityBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER,densities.size() * sizeof(float),densities.data(),GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, densityBuffer);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, predictedPositionBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER,predictedPositions.size() * sizeof(glm::vec2),predictedPositions.data(),GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, predictedPositionBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void ComputeParticleManager::calculateScreenSpacePos() {
        for (int i = 0; i < particleAmount; i++) {
            glm::vec2 zeroTwoSpacePos = glm::vec2(positions[i].x * 2.0f / horizontalBoundary, positions[i].y * 2.0f / verticalBoundary);
            scrSpacePositions[i] = glm::vec2(zeroTwoSpacePos.x - 1.0f, zeroTwoSpacePos.y - 1.0f);
            //printf("zeroTwoSpacePos %d: %f, %f \n", i, scrSpacePositions[i].x, scrSpacePositions[i].y);
        }
    }

    void ComputeParticleManager::SetBoundaries(const int& screenWidth, const int& screenHeight) {
        horizontalBoundary = static_cast<float>(screenWidth) / texelDensity;
        verticalBoundary = static_cast<float>(screenHeight) / texelDensity;
    }
} // core