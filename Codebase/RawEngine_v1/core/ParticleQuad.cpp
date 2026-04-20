//
// Created by Rene on 20-4-2026.
//

#include "ParticleQuad.h"

#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace core {

    ParticleQuad::ParticleQuad() {
        float screenQuadVertices[] = {
            // positions   // texCoords
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f,

            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f,  0.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW);

        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // texcoords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))
        );

        glBindVertexArray(0);
    }

    void ParticleQuad::RenderQuad() const {
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void ParticleQuad::SetPosition(const glm::vec2& newPosition) {
        position = newPosition;
    }
    void ParticleQuad::SetRadius(const glm::vec2& newRadius) {
        radius = newRadius;
    }
} // core