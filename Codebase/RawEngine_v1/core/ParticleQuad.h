//
// Created by Rene on 20-4-2026.
//

#ifndef RAWENGINE_PARTICLEQUAD_H
#define RAWENGINE_PARTICLEQUAD_H
#include <glad/glad.h>
#include <glm/vec2.hpp>

namespace core {
    class ParticleQuad {
        public:
        unsigned int quadVAO{}, quadVBO{};
        glm::vec2 position{};
        glm::vec2 radius{};

        ParticleQuad();

        void RenderQuad() const;

        void SetPosition(const glm::vec2& newPosition);
        void SetRadius(const glm::vec2& newRadius);
    };
} // core

#endif //RAWENGINE_PARTICLEQUAD_H