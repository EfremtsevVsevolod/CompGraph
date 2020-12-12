#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

const std::string FRAGMENT_SHADER_TYPE = "FragmentShader";
const std::string VERTEX_SHADER_TYPE = "VertexShader";

class Shader {
public:
    unsigned int identificator;

public:
    Shader() = default;

    int LoadAndCompile(const std::string path, std::string type) {
        std::string code;
        std::ifstream file;

        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            file.open(path);
            std::stringstream stream;
            stream << file.rdbuf();
            file.close();
            code = stream.str();
        }
        catch (std::ifstream::failure& error) {
            std::cout << "Error in loading shader code (error code: " << error.code()
                << ")" << std::endl;
            return -1;
        }

        const char* raw_code = code.c_str();

        std::cout << "raw_code = " << raw_code << std::endl;

        if (type == VERTEX_SHADER_TYPE) {
            identificator = glCreateShader(GL_VERTEX_SHADER);
        } else if (type == FRAGMENT_SHADER_TYPE) {
            identificator = glCreateShader(GL_FRAGMENT_SHADER);
        }
        else {
            std::cout << "Error in creation shader" << std::endl;
        }
        
        glShaderSource(identificator, 1, &raw_code, NULL);
        glCompileShader(identificator);
        CheckCompileErrors(identificator, type);

        return 0;
    }

    void AttachToProgram(unsigned program_id) {
        glAttachShader(program_id, identificator);
    }

    void Delete() {
        glDeleteShader(identificator);
    }

private:
    void CheckCompileErrors(unsigned int identificator, std::string type) {
        int success_flag;
        char infomation_log[1024];

        glGetShaderiv(identificator, GL_COMPILE_STATUS, &success_flag);
        if (!success_flag) {
            glGetShaderInfoLog(identificator, 1024, NULL, infomation_log);
            std::cout << "Error in compile (type: " << type << ")" << std::endl
                << infomation_log << std::endl;
        }
    }
};

class ShaderProgram {
public:
    unsigned int identificator;

public:
    ShaderProgram() = default;

    void Build(const std::string path_to_vertex_shader,
                    const std::string path_to_fragment_shader) {
        Shader vertex_shader;
        vertex_shader.LoadAndCompile(path_to_vertex_shader, VERTEX_SHADER_TYPE);

        Shader fragment_shader;
        fragment_shader.LoadAndCompile(path_to_fragment_shader, FRAGMENT_SHADER_TYPE);

        identificator = glCreateProgram();
        
        vertex_shader.AttachToProgram(identificator);
        fragment_shader.AttachToProgram(identificator);
        glLinkProgram(identificator);
        CheckLinkErrors(identificator);
        
        vertex_shader.Delete();
        fragment_shader.Delete();
    }

    void Use() {
        glUseProgram(identificator);
    }
    
    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(identificator, name.c_str()), (int)value);
    }
    
    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(identificator, name.c_str()), value);
    }
    
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(identificator, name.c_str()), value);
    }

private:
    void CheckLinkErrors(unsigned int shader) {
        int success_flag;
        char information_log[1024];

        glGetProgramiv(shader, GL_LINK_STATUS, &success_flag);
        if (!success_flag) {
            glGetProgramInfoLog(shader, 1024, NULL, information_log);
            std::cout << "Error in linking: " << std::endl << information_log << std::endl;
        }

    }
};
#endif