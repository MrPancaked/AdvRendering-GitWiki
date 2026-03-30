//
// Created by Rene on 30-3-2026.
//

#ifndef RAWENGINE_PARTICLEMANAGER_H
#define RAWENGINE_PARTICLEMANAGER_H
#include <string>
#include <vector>
#include <glm/vec2.hpp>

namespace core {
    class ParticleManager {
    public:
        int particleAmount;
        std::vector<glm::vec2> particlePositions;

        explicit ParticleManager(int particleAmount) : particleAmount(particleAmount){
            for (int i = 0; i < particleAmount; i++) {
                particlePositions.emplace_back(static_cast<float>(i) * 25.0f, 300.0f);
            }
        };
    };
} // core

#endif //RAWENGINE_PARTICLEMANAGER_H