//
// Created by Rene on 30-3-2026.
//

#include "ParticleManager.h"

namespace core {
    void ParticleManager::UpdateParticles(const float deltaTime) {
        for (int i = 0; i < particleAmount; i++) {
            velocities[i] += glm::vec2(0.0f, -1.0f) * gravity * timeStep * deltaTime;
            positions[i] += velocities[i];
        }
    }
} // core