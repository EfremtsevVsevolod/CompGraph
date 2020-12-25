#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


class ShaderProgram {
public:
    // Constructor with no inizalization (see initialize method)
    ShaderProgram() {
        shader_program = -1;
    }

    GLint GetShaderProgramID() const {
        if (shader_program >= 0) {
            return shader_program;
        } else {
            std::cout << "Error! ShaderProgram::GetShaderProgramID\n\tshader_program_id is not valid";
            return -1;
        }
    }

    void Initialize(const std::vector<std::string> &paths, const std::vector<GLenum> type, int log_info_size = 512) {
        shader_program = glCreateProgram();

        for (int i = 0; i < paths.size(); ++i) {
            std::string code = "";
            std::ifstream file(paths[i], std::ios::in);

            if (file.is_open()) {
                std::string buffer = "";
                while (std::getline(file, buffer)) {
                    code += buffer + "\n";
                }
                file.close();
            } else {
                std::cout << "Error! Shader::InizalizeFromFile" << std::endl
                    << "File not exist or path incorrect" << std::endl;
            }

            unsigned cur_shader = glCreateShader(type[i]);
            const char *sourse = std::data(code);
            glShaderSource(cur_shader, 1, &sourse, nullptr);
            glCompileShader(cur_shader);

            int status;
            std::string log_info;
            log_info.resize(log_info_size);
            glGetShaderiv(cur_shader, GL_COMPILE_STATUS, &status);

            if (!status) {
                glGetShaderInfoLog(cur_shader, log_info_size, nullptr, std::data(log_info));
                std::cout << "Error\n" << "Compilation error\nLOg info:\n" << log_info << std::endl;
            }

 
            glAttachShader(shader_program, cur_shader);
        }

        glLinkProgram(shader_program);

        int status;
        std::string log_info;
        log_info.resize(log_info_size);
        glGetProgramiv(shader_program, GL_LINK_STATUS, &status);

        if (!status) {
            glGetProgramInfoLog(shader_program, 512, nullptr, std::data(log_info));
            std::cout << "Error! ShaderProgram\n\tLinking failed\nLog info:\n" << log_info << std::endl;
        }
    }

    GLint GetLocation(const std::string& name_uniform_var) const {
        return glGetUniformLocation(shader_program, name_uniform_var.c_str());
    }

    void UseShaderProgram() const {
        glUseProgram(shader_program);
    }

    // Set scalar uniform params for all shaders
    void SetInt(const std::string & var_key, GLint var_val) const {
        auto a = glGetUniformLocation(shader_program, var_key.c_str());
        glUniform1i(glGetUniformLocation(shader_program, var_key.c_str()), var_val);
    }

    void SetFloat(const std::string & var_key, GLfloat var_val) const {
        glUniform1f(glGetUniformLocation(shader_program, var_key.c_str()), var_val);
    }

    // Set vector uniform params for all shaders
    void SetVec2(const std::string& var_key, const glm::vec2& var_val) const {
        glUniform2fv(glGetUniformLocation(shader_program, var_key.c_str()), 1, glm::value_ptr(var_val));
    }

    void SetVec3(const std::string& var_key, const glm::vec3& var_val) const {
        glUniform3fv(glGetUniformLocation(shader_program, var_key.c_str()), 1, glm::value_ptr(var_val));
    }

    void SetVec4(const std::string& var_key, const glm::vec4& var_val) const {
        glUniform4fv(glGetUniformLocation(shader_program, var_key.c_str()), 1, glm::value_ptr(var_val));
    }

    // Set matrix uniform params for all shaders
    void SetMat2(const std::string& var_key, const glm::mat2& var_val) const {
        glUniformMatrix2fv(glGetUniformLocation(shader_program, var_key.c_str()), 1, GL_FALSE, glm::value_ptr(var_val));
    }

    void SetMat3(const std::string& var_key, const glm::mat3& var_val) const {
        glUniformMatrix3fv(glGetUniformLocation(shader_program, var_key.c_str()), 1, GL_FALSE, glm::value_ptr(var_val));
    }

    void SetMat4(const std::string& var_key, const glm::mat4& var_val) const {
        glUniformMatrix4fv(glGetUniformLocation(shader_program, var_key.c_str()), 1, GL_FALSE, glm::value_ptr(var_val));
    }

private:
    GLint shader_program;
};
