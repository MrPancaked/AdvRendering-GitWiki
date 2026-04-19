//
// Created by Rene on 3-3-2026.
//

#ifndef RAWENGINE_SHADER_H
#define RAWENGINE_SHADER_H
#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace core {
    class Shader {
    private:
        static GLuint generateShader(const std::string &shaderPath, GLuint shaderType);
        static std::string readFileToString(const std::string &filePath);
    public:
        unsigned int ID;

        Shader(const std::string &vertexPath, const std::string &fragmentPath);
        Shader(const std::string &computePath);

        void use() const;
        void useCompute(const unsigned int& TEXTURE_WIDTH, const unsigned int& TEXTURE_HEIGHT) const;

        GLint GetUniformLocation(const std::string &name) const;

        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setVec2(const std::string &name, const glm::vec2 &value) const;
        void setTexture(const std::string &name, const GLuint &textureID, const GLuint& unit) const;
        void setMat4(const std::string &name, const glm::mat4 &mat) const;

        void DeleteShaderProgram() const;

    };
} // core

#endif //RAWENGINE_SHADER_H