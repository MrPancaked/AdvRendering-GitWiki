//
// Created by Rene on 29-3-2026.
//

#ifndef RAWENGINE_PARTICLE_H
#define RAWENGINE_PARTICLE_H
#include <glm/vec2.hpp>

namespace core {
    class Particle {
    private:
    public:
        glm::vec2 position;

        Particle(const glm::vec2 position)
            : position(position){}
    };
}



#endif //RAWENGINE_PARTICLE_H