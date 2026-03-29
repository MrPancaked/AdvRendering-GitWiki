//
// Created by Rene on 3-3-2026.
//

#include "Shader.h"

#include <glm/fwd.hpp>


namespace core {
    std::string Shader::readFileToString(const std::string &filePath) {
        std::ifstream fileStream(filePath, std::ios::in);
        if (!fileStream.is_open()) {
            printf("Could not open file: %s\n", filePath.c_str());
            return "";
        }
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        return buffer.str();
    }

    GLuint Shader::generateShader(const std::string &shaderPath, GLuint shaderType) {
        printf("Loading shader: %s\n", shaderPath.c_str());
        const std::string shaderText = readFileToString(shaderPath);
        const GLuint shader = glCreateShader(shaderType);
        const char *s_str = shaderText.c_str();
        glShaderSource(shader, 1, &s_str, nullptr);
        glCompileShader(shader);
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            printf("Error! Shader issue [%s]: %s\n", shaderPath.c_str(), infoLog);
        }
        return shader;
    }
    Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
        int success;
        char infoLog[512];

        const unsigned int vertex = generateShader(vertexPath, GL_VERTEX_SHADER);
        const unsigned int fragment = generateShader(fragmentPath, GL_FRAGMENT_SHADER);

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        // print linking errors if any
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void Shader::use() const {
        glUseProgram(ID);
    }

    GLint Shader::GetUniformLocation(const std::string &name) const {
        GLint location = glGetUniformLocation(ID, name.c_str());
        if (location == -1) {
            std::cout << "Warning: uniform '" << name << "' not found!\n";
        }
        return location;
    }

    //the setter functions all use GetUniformLocation() which needs to be called every time
    //instead of saving the location somewhere first and then just assigning values to that location
    //This solution increases readability at the cost of performance
    void Shader::setBool(const std::string &name, bool value) const{
        glUniform1i(GetUniformLocation(name), (int)value);
    }
    void Shader::setInt(const std::string &name, int value) const{
        glUniform1i(GetUniformLocation(name), value);
    }
    void Shader::setFloat(const std::string &name, float value) const{
        glUniform1f(GetUniformLocation(name), value);
    }
    void Shader::setTexture(const std::string &name, const GLuint &textureID, const GLuint& unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(GetUniformLocation(name), unit);
    }
    void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
    }
    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::DeleteShaderProgram() const {
        glDeleteProgram(ID);
    }


} // core