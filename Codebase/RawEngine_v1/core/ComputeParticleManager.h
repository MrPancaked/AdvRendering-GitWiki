//
// Created by Rene on 20-4-2026.
//

#ifndef RAWENGINE_COMPUTEPARTICLEMANAGER_H
#define RAWENGINE_COMPUTEPARTICLEMANAGER_H
#include "imgui_impl_opengl3_loader.h"

namespace core {
    class ComputeParticleManager {
        public:
            ComputeParticleManager();
        private:
            GLuint positionBuffer{};
            GLuint velocityBuffer{};

    };
} // core

#endif //RAWENGINE_COMPUTEPARTICLEMANAGER_H